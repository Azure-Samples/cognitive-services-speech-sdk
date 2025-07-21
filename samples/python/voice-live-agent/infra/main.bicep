targetScope = 'subscription'

@minLength(1)
@maxLength(64)
@description('Name of the the environment which is used to generate a short unique hash used in all resources.')
param environmentName string

@minLength(1)
@description('Primary location for all resources (filtered on available regions for Azure Open AI Service).')
@allowed([
  'eastus2'
  'swedencentral'
])
param location string

var uniqueSuffix = substring(uniqueString(subscription().id, environmentName), 0, 5)

param appExists bool

var tags = {'azd-env-name': environmentName }
var rgName = 'rg-${environmentName}-${uniqueSuffix}'
// TODO: Allow user to select in runtime
var modelName = 'gpt-4o-mini'

resource rg 'Microsoft.Resources/resourceGroups@2023-07-01' = {
  name: rgName
  location: location
  tags: tags
}

// [ User Assigned Identity for App to avoid circular dependency ]
module appIdentity './modules/identity.bicep' = {
  name: 'uami'
  scope: rg
  params: {
    location: location
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
  }
}

var sanitizedEnvName = toLower(replace(replace(replace(replace(environmentName, ' ', '-'), '--', '-'), '[^a-zA-Z0-9-]', ''), '_', '-'))
var logAnalyticsName = take('log-${sanitizedEnvName}-${uniqueSuffix}', 63)
var appInsightsName = take('insights-${sanitizedEnvName}-${uniqueSuffix}', 63)
module monitoring 'modules/monitoring/monitor.bicep' = {
  name: 'monitor'
  scope: rg
  params: {
    logAnalyticsName: logAnalyticsName
    appInsightsName: appInsightsName
    tags: tags
  }
}
module registry 'modules/containerregistry.bicep' = {
  name: 'registry'
  scope: rg
  params: {
    location: location
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    identityName: appIdentity.outputs.name
    tags: tags
  }
  dependsOn: [ appIdentity ]
}


module aiServices 'modules/aiservices.bicep' = {
  name: 'ai-foundry-deployment'
  scope: rg
  params: {
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    identityId: appIdentity.outputs.identityId
    tags: tags
  }
  dependsOn: [ appIdentity ]
}

module acs 'modules/acs.bicep' = {
  name: 'acs-deployment'
  scope: rg
  params: {
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    tags: tags
  }
}

var keyVaultName = toLower(replace('kv-${environmentName}-${uniqueSuffix}', '_', '-'))
var sanitizedKeyVaultName = take(toLower(replace(replace(replace(replace(keyVaultName, '--', '-'), '_', '-'), '[^a-zA-Z0-9-]', ''), '-$', '')), 24)
module keyvault 'modules/keyvault.bicep' = {
  name: 'keyvault-deployment'
  scope: rg
  params: {
    location: location
    keyVaultName: sanitizedKeyVaultName
    tags: tags
    aiServicesKey: aiServices.outputs.aiServicesKey
    acsConnectionString: acs.outputs.acsConnectionString
  }
  dependsOn: [ appIdentity, acs, aiServices ]
}

// Add role assignments 
module RoleAssignments 'modules/roleassignments.bicep' = {
  scope: rg
  name: 'role-assignments'
  params: {
    identityPrincipalId: appIdentity.outputs.principalId
    aiServicesId: aiServices.outputs.aiServicesId
    keyVaultName: sanitizedKeyVaultName
  }
  dependsOn: [ keyvault, appIdentity ] 
}

module containerapp 'modules/containerapp.bicep' = {
  name: 'containerapp-deployment'
  scope: rg
  params: {
    location: location
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    tags: tags
    exists: appExists
    identityId: appIdentity.outputs.identityId
    containerRegistryName: registry.outputs.name
    aiServicesEndpoint: aiServices.outputs.aiServicesEndpoint
    modelDeploymentName: modelName
    aiServicesKeySecretUri: keyvault.outputs.aiServicesKeySecretUri
    acsConnectionStringSecretUri: keyvault.outputs.acsConnectionStringUri
    logAnalyticsWorkspaceName: logAnalyticsName
  }
  dependsOn: [keyvault, RoleAssignments]
}


// OUTPUTS will be saved in azd env for later use
output AZURE_LOCATION string = location
output AZURE_TENANT_ID string = tenant().tenantId
output AZURE_RESOURCE_GROUP string = rg.name
output AZURE_USER_ASSIGNED_IDENTITY_ID string = appIdentity.outputs.identityId
output AZURE_USER_ASSIGNED_IDENTITY_CLIENT_ID string = appIdentity.outputs.clientId

output AZURE_CONTAINER_REGISTRY_ENDPOINT string = registry.outputs.loginServer

output SERVICE_API_ENDPOINTS array = ['${containerapp.outputs.containerAppFqdn}/acs/incomingcall']

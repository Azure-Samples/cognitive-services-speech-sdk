targetScope = 'subscription'

@minLength(1)
@description('Primary location for all resources (filtered on available regions for Azure Open AI Service).')
@allowed([
  'swedencentral'
  'eastus2'
])
param location string

// @description('Environment name')
// param environmentName string
var environmentName = 'voiceagenttemp'

// var uniqueSuffix = substring(uniqueString(subscription().id, environmentName), 0, 5)
var uniqueSuffix = 'fmnp7'

param exists bool = false

var tags = {
  environment: environmentName
  application: 'azure-voice-agent'
}
var rgName = 'rg-${environmentName}-${uniqueSuffix}'
var modelName = 'gpt-4o-mini'

resource rg 'Microsoft.Resources/resourceGroups@2023-07-01' = {
  name: rgName
  location: location
  tags: tags
}

module aiServices 'modules/aiservices.bicep' = {
  name: 'ai-foundry-deployment'
  scope: rg
  params: {
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    tags: tags
  }
}

module acs 'modules/acs.bicep' = {
  name: 'acs-deployment'
  scope: rg
  params: {
    location: location
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    tags: tags
  }
}

module keyvault 'modules/keyvault.bicep' = {
  name: 'keyvault-deployment'
  scope: rg
  params: {
    location: location
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    tags: tags
    aiServicesKey: aiServices.outputs.aiServicesKey
    acsConnectionString: acs.outputs.acsConnectionString
  }
}

module containerapp 'modules/containerapp.bicep' = {
  name: 'containerapp-deployment'
  scope: rg
  params: {
    location: location
    environmentName: environmentName
    uniqueSuffix: uniqueSuffix
    tags: tags
    exists: exists
    aiServicesEndpoint: aiServices.outputs.aiServicesEndpoint
    modelDeploymentName: modelName
    aiServicesKeySecretUri: keyvault.outputs.aiServicesKeySecretUri
    acsConnectionStringSecretUri: keyvault.outputs.acsConnectionStringUri
  }
}

module containerAppRoleAssignments 'modules/containerapp-roles.bicep' = {
  name: 'containerapp-role-assignments'
  scope: rg
  params: {
    containerAppPrincipalId: containerapp.outputs.containerAppPrincipalId
    aiServicesId: aiServices.outputs.aiServicesId
    keyVaultName: keyvault.outputs.keyVaultName
  }
}

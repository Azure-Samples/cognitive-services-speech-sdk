param environmentName string
param uniqueSuffix string
param identityId string
param tags object

// Voice live api only supported on two regions now 
var location string = 'swedencentral'
var aiServicesName string = 'aiServices-${environmentName}-${uniqueSuffix}'

@allowed([
  'S0'
])
param sku string = 'S0'

resource aiServices 'Microsoft.CognitiveServices/accounts@2023-05-01' = {
  name: aiServicesName
  location: location
  identity: {
    type: 'UserAssigned'
    userAssignedIdentities: { '${identityId}': {} }
  }
  sku: {
    name: sku
  }
  kind: 'AIServices'
  tags: tags
  properties: {
    publicNetworkAccess: 'Enabled'
    networkAcls: {
      defaultAction: 'Allow'
    }
    disableLocalAuth: false
    customSubDomainName: 'domain-${environmentName}-${uniqueSuffix}' 
  }
}

@secure()
output aiServicesKey string = aiServices.listKeys().key1
output aiServicesEndpoint string = aiServices.properties.endpoint
output aiServicesId string = aiServices.id
output aiServicesName string = aiServices.name

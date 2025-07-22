param environmentName string
param uniqueSuffix string
param tags object = {}

var acsName string = 'acs-${environmentName}-${uniqueSuffix}'

resource acs 'Microsoft.Communication/communicationServices@2025-05-01-preview' = {
  name: acsName
  location: 'global'
  tags: tags
  properties: {
    dataLocation: 'United States'
  }
}

@secure()
output acsConnectionString string = acs.listKeys().primaryConnectionString
output acsResourceId string = acs.id

param exists bool
param name string

resource existingApp 'Microsoft.App/containerApps@2023-04-01-preview' existing = if (exists) {
  name: name
}

output containers array = exists ? existingApp.properties.template.containers : []

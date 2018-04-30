MOXYGENBEGIN {{anchor refid}}
---
ms.service: cognitive-services
ms.technology: Speech
ms.topic: reference
ms.date: 05/07/2018
ms.author: wolfma
---
{{anchor refid}}
# {{kind}} {{name}}

{{#if basecompoundref}}
```
{{kind}} {{name}}
  {{#each basecompoundref}}
  : {{prot}} {{name}}
  {{/each}}
```
{{/if}}

{{briefdescription}}

{{detaileddescription}}

## Members

{{#each filtered.compounds}}
{{anchor refid}}
#### {{name}}

Syntax: `{{plain}}`

{{briefdescription}}

{{detaileddescription}}
{{/each}}

{{#each filtered.members}}
{{anchor refid}}
#### {{name}}

Syntax: `{{plain}};`

{{#if enumvalue}}
 Values                         | Descriptions
--------------------------------|---------------------------------------------
{{#each enumvalue}}{{cell name}}            | {{cell summary}}
{{/each}}
{{/if}}

{{briefdescription}}

{{detaileddescription}}

{{/each}}
MOXYGENEND {{anchor refid}}

MOXYGENBEGIN {{anchor refid}}
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

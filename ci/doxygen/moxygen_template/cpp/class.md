MOXYGENBEGIN {{anchor refid}}
{{anchor refid}}
# {{kind}} `{{name}}`

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

## Summary

 Members                        | Descriptions
--------------------------------|---------------------------------------------
{{#each filtered.compounds}}{{cell proto}}        | {{cell summary}}
{{/each}}{{#each filtered.members}}{{cell proto}} | {{cell summary}}
{{/each}}

## Members

{{#each filtered.compounds}}
{{anchor refid}}
#### {{title proto}}

{{briefdescription}}

{{detaileddescription}}
{{/each}}

{{#each filtered.members}}
{{anchor refid}}
#### {{title proto}}

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

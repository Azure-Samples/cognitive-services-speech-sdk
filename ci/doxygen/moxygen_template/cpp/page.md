{{anchor refid}}
# {{kind}} `{{name}}`

{{briefdescription}}

{{detaileddescription}}

{{#if filtered.members}}

## Summary

 Members                        | Descriptions
--------------------------------|---------------------------------------------
{{#each filtered.members}}[{{kind}} {{name}}](#{{refid}})            | {{cell summary}}
{{/each}}{{#each filtered.compounds}}[{{kind}} {{name}}](#{{refid}}) | {{cell summary}}
{{/each}}

## Members

{{#each filtered.members}}
{{anchor refid}}
#### {{kind}} {{name}}

{{#if enumvalue}}
 Values                         | Descriptions
--------------------------------|---------------------------------------------
{{#each enumvalue}}{{cell name}}            | {{cell summary}}
{{/each}}
{{/if}}

{{briefdescription}}

{{detaileddescription}}

{{/each}}
{{/if}}

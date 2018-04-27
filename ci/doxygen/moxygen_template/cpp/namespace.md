MOXYGENBEGIN {{anchor refid}}
{{anchor refid}}
# {{kind}} `{{name}}`

{{briefdescription}}

{{detaileddescription}}

## Summary

 Members                        | Descriptions
--------------------------------|---------------------------------------------
{{#each filtered.members}}{{cell proto}}            | {{cell summary}}
{{/each}}{{#each filtered.compounds}}{{cell proto}} | {{cell summary}}
{{/each}}

{{#if filtered.members}}
## Members

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
{{/if}}
MOXYGENEND {{anchor refid}}

MOXYGENBEGIN dummy-index-ref
# Summary

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

{{briefdescription}}

{{#if enumvalue}}
 Values                         | Descriptions
--------------------------------|---------------------------------------------
{{#each enumvalue}}{{cell name}}            | {{cell summary}}
{{/each}}
{{/if}}

{{detaileddescription}}

{{/each}}
{{/if}}
MOXYGENEND dummy-index-ref

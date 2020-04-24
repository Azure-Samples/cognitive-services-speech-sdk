call az login -o none
call az extension add --name "azure-devops"
call az devops configure -d organization"="https://msasg.visualstudio.com/" project="Skyman"
call az devops configure -l

if NOT "%1" equ "" (
cd \Release_Staging\%1
)
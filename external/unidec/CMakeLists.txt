project(unidec-dep)

if(NOT DEFINED VSTS_TOKEN)
    message(FATAL_ERROR "Missing VSTS access token.")
else()
    file(COPY ${UNIDEC}/nuget.config DESTINATION ${CARBON_EXTERNAL}/unidec)

    execute_process(COMMAND nuget.exe sources add -name Richland -source https://msasg.pkgs.visualstudio.com/_packaging/Bing/nuget/v3/index.json -username carbon -password ${VSTS_TOKEN} -ConfigFile ${CARBON_EXTERNAL}/unidec/nuget.config)
    execute_process(COMMAND nuget.exe sources add -name UnidecModel -source https://msasg.pkgs.visualstudio.com/_packaging/CarbonOfflineModel/nuget/v3/index.json -username carbon -password ${VSTS_TOKEN} -ConfigFile ${CARBON_EXTERNAL}/unidec/nuget.config)
    execute_process(COMMAND nuget.exe install ${UNIDEC}/packages.config -OutputDirectory ${CARBON_EXTERNAL}/unidec -ExcludeVersion -ConfigFile ${CARBON_EXTERNAL}/unidec/nuget.config)

    # Prevent hits from CredentialScanner
    unset(VSTS_TOKEN CACHE)
    file(REMOVE ${CARBON_EXTERNAL}/unidec/nuget.config)
endif()

This directory is used to generate docFx based documentation

- install docFx https://dotnet.github.io/docfx/

- In this folder run `docFx --intermediateFolder ..\..\build\docfx` or use the `build.cmd` file. This will generate the docFx generated documentation in `<repo>/build/dotfx` as the default `_site`

- Serve the side under http://localhost:8080 by running `dotfx serve ..\..\build\dotfx\_site` or running `serve.cmd`.

- To add new articles, add them to the `<repo>\docs\articles`, update `index.md` in `<repo>\docs` as needed to link new articles.

- To add new languages, update `docfx.json` in `<repo>\build\dotfx`


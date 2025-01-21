# Examples to use Batch Synthesis

The Batch synthesis API provides asynchronous synthesis of long-form text to speech. The functionality is exposed through a REST API and is easy to access from many programming languages. The samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly instead.

For a detailed explanation see the [batch synthesis documentation](https://learn.microsoft.com/azure/ai-services/speech-service/batch-synthesis) and the `README.md` in the language specific subdirectories.

Available samples:

| Language | Directory | Description |
| ---------- | -------- | ----------- |
| C# | [csharp](csharp) | C# calling batch synthesis REST API through System.Net.Http |
| Python | [python](python) | Python client calling batch synthesis REST API |
| Java | [java](java) | Java client calling batch synthesis REST API |

## Note

The sample text [*The Great Gatsby* Chapter 1](./Gatsby-chapter1.txt) is an open-domain book and downloaded from [gutenberg](https://www.gutenberg.org/ebooks/64317)

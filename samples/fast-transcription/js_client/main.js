const https = require('https');
const fs = require('fs');
const FormData = require('form-data');
const path = require('path');

// Add Credentials 
const SUBSCRIPTION_KEY= "YourSubscriptionKey";
const SERVICE_REGION=  "YourServiceRegion";


// fast transcription settings
const LOCALE = ["en-US"];
const PROFANITYFILTER = "Masked";
const DIARIZATIONSETTINGS = { minSpeakers: 1, maxSpeakers: 4 };

// Get transcription properties payload
function getTranscriptionProperties() {
    return {
        locales: LOCALE,
        properties: {
            profanityFilterMode: PROFANITYFILTER,
            channels: [0], // Mono-speaker diarization
            diarizationSettings: DIARIZATIONSETTINGS
        }
    };
}

// transcribe the code from a local file
function transcribeFromLocal(audioFilePath, transcriptionsFilePath) {
  // Check if the file exists
  if (!fs.existsSync(audioFilePath)) {
      console.error("File does not exist at the specified path:", audioFilePath);
      return;
  }

  // create the form data for Multipart API call
  const formData = new FormData();
  formData.append('audio', fs.createReadStream(audioFilePath), path.basename(audioFilePath));
  formData.append('definition', JSON.stringify(getTranscriptionProperties()), { contentType: 'application/json' });

  // set up the request options
  const options = {
      hostname: `${SERVICE_REGION}.api.cognitive.microsoft.com`,
      path: '/speechtotext/transcriptions:transcribe?api-version=2024-05-15-preview',
      method: 'POST',
      headers: {
          'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY,
          ...formData.getHeaders()
      }
  };

  // Send the requests
  const req = https.request(options, (res) => {
      let data = '';

      res.on('data', (chunk) => {
          data += chunk;
      });

      res.on('end', () => {
          if (res.statusCode === 200) {
              console.log("Transcription successful!");

              // Save the transcription response to a file
              fs.writeFileSync(transcriptionsFilePath, JSON.stringify(JSON.parse(data), null, 4));
              console.log(`Json response saved to ${transcriptionsFilePath}`);
          } else {
              console.error(`Error: ${res.statusCode} - ${res.statusMessage}`);
              console.error(data);
          }
      });
  });

  req.on('error', (e) => {
      console.error(`Problem with request: ${e.message}`);
  });

  // Pipe the form data to the request
  formData.pipe(req);
}

// main function 
function main() {
  const args = process.argv.slice(2);
  const inputDir = args[0] || 'test_call_audio.wav'; // test audio file - change to YourAudio_FilePath
  const outputDir = args[1] || 'transcription_response.json'; // test output JSON file

  transcribeFromLocal(inputDir, outputDir);
}


main();



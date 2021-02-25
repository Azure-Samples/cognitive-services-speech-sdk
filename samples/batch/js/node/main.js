const { TranscriptionProperties } = require('speech_to_text_api_v30');
var SpeechToTextApiV30 = require('speech_to_text_api_v30');
var sleep = require('system-sleep');
var defaultClient = SpeechToTextApiV30.ApiClient.instance;
const request = require('request');

// Create API Instance
var apiInstance = new SpeechToTextApiV30.DefaultApi();
// Your subscription key and region for the speech service

const API_KEY=""
const NAME = "Simple transcription"
const DESCRIPTION = "Simple transcription description"
const LOCALE = "en-US"

// Provide the uri of the audio file 
const RECORDINGS_BLOB_URI = ""

// Provide the uri of a container with audio files for transcribing all of them with a single request
const RECORDINGS_CONTAINER_URI = ""

var callbackGetTrans = function(error, data, response) {
  if (error) {
    console.error(error);
  } else {
    console.log('Callback Get Transcription API called successfully. Returned data: ' + JSON.stringify(data));
    completed=false ;
    var tid = data.self.substring(data.self.lastIndexOf("/")+1);
    console.log("Transcription Id " + tid);
    status = _checkComplete(tid);

    if (status == "Succeeded")
    {
        var opts = _getTranscriptionFiles();
        apiInstance.getTranscriptionFiles(tid,opts,callbackGetTransFiles);
    }
    else if(data.status == "Failed")
           console.log("Transcription failed: {transcription.properties.error.message}")
  }
};

var callbackGetTransFiles = function(error, data, response) {
  if (error) {
    console.error(error);
  } else {
   console.log('callbackGetTransFiles API called successfully. Returned data: ' + JSON.stringify(data));
   pag_files = data.values;
   for (var i = 0; i < pag_files.length; i++)
   {
     var file_data = pag_files[i];
     var url = file_data.links.contentUrl; 
     if(file_data.kind !='Transcription')
        continue;
     else{
        request(url, { json: true }, (err, res, body) => {
            if (err) { 
                return console.log(err); 
            }
            else {
              console.log("\n\n\n Name of file : " + file_data.name);
              console.log("\n Transcription File Content is : " + JSON.stringify(body));
            }
        });
        
     }
   }
  }
};

// uncomment to execute 
// _transcribe();
// _deleteAllTranscriptions();

function _transcribe()
{
    
    // Configure API key authorization: apiKeyHeader
    var apiKeyHeader = defaultClient.authentications['apiKeyHeader'];
    apiKeyHeader.apiKey = API_KEY;
    // Configure API key authorization: apiKeyQuery
    var apiKeyQuery = defaultClient.authentications['apiKeyQuery'];
    apiKeyQuery.apiKey = API_KEY;

    defaultClient.basePath = 'https:// centralindia.api.cognitive.microsoft.com/speechtotext/v3.0'.replace(/\/+$/, '');

    // Uncomment this block to transcribe one file.
    // var opts = _transcribeFromSingleBlob(RECORDINGS_BLOB_URI);

    // Uncomment this block to transcribe all files from a container.
    var opts = _transcribeFromContainer(RECORDINGS_CONTAINER_URI);
    
    apiInstance.createTranscription(opts, function(error, data, response) {
        if (error) {
          console.error(error);
        } 
        else {
          var completed = false;
          var tid = data.self.substring(data.self.lastIndexOf("/")+1);
          console.log("Transcription ID created is : "+tid);
          apiInstance.getTranscription(tid,callbackGetTrans);
          
        }
      });
    
}// end of _transcribe()


function _deleteAllTranscriptions(){
    
  // Delete all transcriptions associated with your speech resource.
  console.log("Deleting all existing completed transcriptions.")
  // Configure API key authorization: apiKeyHeader
  var apiKeyHeader = defaultClient.authentications['apiKeyHeader'];
  apiKeyHeader.apiKey = API_KEY;
  // Configure API key authorization: apiKeyQuery
  var apiKeyQuery = defaultClient.authentications['apiKeyQuery'];
  apiKeyQuery.apiKey = API_KEY;
  
  defaultClient.basePath = 'https:// centralindia.api.cognitive.microsoft.com/speechtotext/v3.0'.replace(/\/+$/, '');

  // get all transcriptions for the subscription
  var opts = { 
  };
  var callback = function(error, data, response) {
        if (error) {
          console.error(error);
        } else {
          // uncomment to display the response of Delete All transcriptions
          // console.log('Delete all transcriptions API called successfully. Returned data: ' + JSON.stringify(data));
          pag_files = data.values;
          console.log(pag_files.length);
          for (var i = 0; i < pag_files.length; i++){
            var file_data = pag_files[i];
            var tid = file_data.self.substring(file_data.self.lastIndexOf("/")+1);
            if(file_data.status=='Succeeded' || file_data.status == 'Failed'){
               console.log("Deleting Transcription with status failed or success " + tid);
               var callback = function(error, data, response) {
                if (error) {
                  console.error(error);
                } else {
                  console.log('Transcription deleted - API called successfully.'+tid);
                }
              };
              apiInstance.deleteTranscription(tid, callback);
            }
            else{
                console.log("Status running" + file_data.status);
            }
        }
      };
    }// end else
      apiInstance.getTranscriptions(opts, callback);
}// end deleteAllTranscriptions



function _getTranscriptionFiles()
{
    var opts = { 
        // Transcription | The details of the new transcription.
            'transcription' : new SpeechToTextApiV30.Transcription()
        }
        return opts;
}

function _transcribeFromSingleBlob(uri){
    var opts = { 
        // Transcription | The details of the new transcription.
            'transcription' : new SpeechToTextApiV30.Transcription()
        }
    
        opts.transcription = 
        {
          "contentUrls": [
          uri
          ],
          "properties": {
          "wordLevelTimestampsEnabled": true
          },
          "locale": LOCALE,
          "displayName": NAME + new Date(),
          "description" : DESCRIPTION
        };
        return opts;
}// end of _transcribeFromSingleBlob

function _transcribeFromContainer(uri){
    console.log("_transcribeFromContainer");
    var opts = { 
        // Transcription | The details of the new transcription.
            'transcription' : new SpeechToTextApiV30.Transcription()
        }
        opts.transcription = 
        {
          "contentContainerUrl": uri,
          "properties": {
          "wordLevelTimestampsEnabled": true
          },
          "locale": LOCALE,
          "displayName": NAME + new Date(),
          "description" : DESCRIPTION
        };
        return opts;
}// end of _transcribeFromSingleBlob


function _checkComplete(tid)
{
  var status ; 
  var completed = false;
  while(completed!=true)
  {
  // wait for 5 seconds before refreshing the transcription status
    sleep(5000);
    
    apiInstance.getTranscription(tid, function(error, data, response) 
    {

      console.log("Transcription Status is ",data.status);
      status = data.status;

      if (error) {
        console.error(error);
        completed = true;
      } 
      else {
        if (status == "Failed" || status == "Succeeded")
          completed = true; 
      }
    }
    );
  
  }// end while
  return status; 
  
}
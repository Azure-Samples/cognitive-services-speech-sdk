

const { TranscriptionProperties } = require('speech_to_text_api_v30');
var SpeechToTextApiV30 = require('speech_to_text_api_v30');
var sleep = require('system-sleep');
var defaultClient = SpeechToTextApiV30.ApiClient.instance;
const request = require('request');

   //Create API Instance
   var apiInstance = new SpeechToTextApiV30.DefaultApi();
// Your subscription key and region for the speech service
const SUBSCRIPTION_KEY = ""

const SERVICE_REGION = "centralindia"

const NAME = "Simple transcription"
const DESCRIPTION = "Simple transcription description"

const LOCALE = "en-US"
const RECORDINGS_BLOB_URI = ""

// Provide the uri of a container with audio files for transcribing all of them with a single request
const RECORDINGS_CONTAINER_URI = ""

//Set model information when doing transcription with custom models
const MODEL_REFERENCE = 'None' ; // guid of a custom model

var callback2 = function(error, data, response) {
        var completed = "False";
        if (error) {
          console.error(error);
        } else {
          console.log('API called successfully. Returned data ' );
          var jsonParsed = JSON.parse(JSON.stringify(data));
          console.log(jsonParsed.status);
          var trans_status = jsonParsed.status;

          if (trans_status == "Failed" || trans_status == "Succeeded")
          {
            completed = "True"
          }
        if (trans_status == "Succeeded")
          {
           
          }
        else if (trans_status == "Failed")
            console.log("Transcription failed: {transcription.properties.error.message}")

        }
      };

 var callback = function(error, data, response) {
  if (error) {
    console.error(error);
  } else {
   console.log('API called successfully. Returned data: ' + JSON.stringify(data));

  }
};

var callbackGetTrans = function(error, data, response) {
  if (error) {
    console.error(error);
  } else {
   console.log('Callback Get API called successfully. Returned data: ' + JSON.stringify(data));
    completed=false ;
    var tid = data.self.substring(data.self.lastIndexOf("/")+1);
    console.log("Trans Id " + tid);
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
   console.log(pag_files.length);
   for (var i = 0; i < pag_files.length; i++){
     var file_data = pag_files[i];
     var url = file_data.links.contentUrl;
    
    console.log (" url " + url);
     if(file_data.kind!='Transcription')
        continue;
     else{
        request(url, { json: true }, (err, res, body) => {
            if (err) { 
                return console.log(err); 
            }
         //   console.log("***************************************************************  response "+JSON.stringify(body));
            console.log("***************************************************************************");

            console.log(body);
            //content.decode('utf-8')
        });
        
     }
   }
  }
};
        
//_transcribe();
//_deleteAllTranscriptions();

function _transcribe()
{
    
    // Configure API key authorization: apiKeyHeader
    var apiKeyHeader = defaultClient.authentications['apiKeyHeader'];
    apiKeyHeader.apiKey = '';
    // Configure API key authorization: apiKeyQuery
    var apiKeyQuery = defaultClient.authentications['apiKeyQuery'];
    apiKeyQuery.apiKey = '';

    //Create API Instance
    //var apiInstance = new SpeechToTextApiV30.DefaultApi();

    const subscriptionKey = "{your-subscription-key-here}";
    const serviceRegion = "centralindia"; // e.g., "westus"
    defaultClient.basePath = 'https://centralindia.api.cognitive.microsoft.com/speechtotext/v3.0'.replace(/\/+$/, '');

    // Uncomment this block to transcribe one file.
    //var opts = _transcribeFromSingleBlob(RECORDINGS_BLOB_URI);

    // Uncomment this block to use custom models for transcription.
    // transcription_definition = transcribe_with_custom_model(api, RECORDINGS_BLOB_URI, properties)

    // Uncomment this block to transcribe all files from a container.
    var opts = _transcribeFromContainer(RECORDINGS_CONTAINER_URI);
    
    apiInstance.createTranscription(opts, function(error, data, response) {
        if (error) {
          console.error(error);
        } 
        else {
          var completed = false;
          var tid = data.self.substring(data.self.lastIndexOf("/")+1);
          console.log("Transaction ID is :"+tid);
          apiInstance.getTranscription(tid,callbackGetTrans);
          
        }
      });
    
}//end of _transcribe()


function _deleteAllTranscriptions(){
    
    //Delete all transcriptions associated with your speech resource.
    console.log("Deleting all existing completed transcriptions.")
    // Configure API key authorization: apiKeyHeader
    var apiKeyHeader = defaultClient.authentications['apiKeyHeader'];
    apiKeyHeader.apiKey = '';
    // Configure API key authorization: apiKeyQuery
    var apiKeyQuery = defaultClient.authentications['apiKeyQuery'];
    apiKeyQuery.apiKey = '';

    //Create API Instance
    //var apiInstance = new SpeechToTextApiV30.DefaultApi();

    const subscriptionKey = "{your-subscription-key-here}";
    const serviceRegion = "centralindia"; // e.g., "westus"
    defaultClient.basePath = 'https://centralindia.api.cognitive.microsoft.com/speechtotext/v3.0'.replace(/\/+$/, '');

    // get all transcriptions for the subscription
     var opts = { 
        'skip': 56, // Number | Format - int32. Number of transcriptions that will be skipped.
        'top': 56 // Number | Format - int32. Number of transcriptions that will be included after skipping.
      };
      
      
      var callback = function(error, data, response) {
        if (error) {
          console.error(error);
        } else {
          //console.log('API called successfully. Returned data: ' + JSON.stringify(data));
          pag_files = data.values;
          console.log(pag_files.length);
          for (var i = 0; i < pag_files.length; i++){
            var file_data = pag_files[i];
            var tid = file_data.self.substring(file_data.self.lastIndexOf("/")+1);
           console.log (" tid " + tid);
            if(file_data.status=='Succeeded' || file_data.status == 'Failed'){
               console.log("status failed or success");
               var callback = function(error, data, response) {
                if (error) {
                  console.error(error);
                } else {
                  console.log('API called successfully.');
                }
              };
              apiInstance.deleteTranscription(tid, callback);
                

            }
            else{
                console.log("status running" + file_data.status);
            }
        }
      };
    }//end else
      apiInstance.getTranscriptions(opts, callback);
}//end deleteAllTranscriptions



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
}//end of _transcribeFromSingleBlob


//To be worked on 
function _transcribeWithCustomModel(api, uri){

    //Transcribe a single audio file located at `uri` using the settings specified in `properties`
    //using the base model for the specified locale.
    
    // Model information (ADAPTED_ACOUSTIC_ID and ADAPTED_LANGUAGE_ID) must be set above.
    if (MODEL_REFERENCE == 'None')
    {
        console.log("Custom model ids must be set when using custom models");
    }
    //model = apiInstance.getModel(MODEL_REFERENCE);
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
          "description" : DESCRIPTION,
           "model" :  model
        };
        return opts;

    
}//_transcribeWithCustomModel

function _transcribeFromContainer(uri){
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
}//end of _transcribeFromSingleBlob


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
  
  }//end while
  return status; 
  
}
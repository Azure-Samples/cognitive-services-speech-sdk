// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

(function() {
  "use strict";
  var SpeechToTextApiV30 = require('speech_to_text_api_v30');
  var sleep = require('system-sleep');
  var defaultClient = SpeechToTextApiV30.ApiClient.instance;
  var request = require('request');

  // Create API Instance
  var apiInstance = new SpeechToTextApiV30.DefaultApi();
  // Your subscription key and region for the speech service

  var API_KEY=""
  var NAME = "Simple transcription"
  var DESCRIPTION = "Simple transcription description"
  var LOCALE = "en-US"
  // provide the service region
  var SERVICE_REGION = "your service region"
  var DEFAULTPATH = 'https://'+SERVICE_REGION+'.api.cognitive.microsoft.com/speechtotext/v3.0'

  // Provide the SAS URI of the audio file stored in Azure Blob Storage
  var RECORDINGS_BLOB_URI = ""

  // Provide the SAS URI pointing to a container in Azure Blob Storage for transcribing all of them with a single request
  var RECORDINGS_CONTAINER_URI = ""
  var callbackGetTrans = function(error, data, response) {
    if (error) {
      console.error(error);
    } 
    else {
      console.log('Callback Get Transcription API called successfully. Returned data: ' + JSON.stringify(data));
      completed=false ;
      var tid = data.self.substring(data.self.lastIndexOf("/")+1);
      console.log("Transcription Id " + tid);
      status = _checkComplete(tid);
      if (status == "Succeeded"){
        var opts = _getTranscriptionFiles();
        apiInstance.getTranscriptionFiles(tid,opts,callbackGetTransFiles);
      } 
      else if (data.status == "Failed")
        console.log("Transcription failed: {transcription.properties.error.message}")
    }// end else
  };// end callbackGetTrans

  var callbackGetTransFiles = function(error, data, response) {
    if (error) {
      console.error(error);
    } 
    else 
    {
      console.log('callbackGetTransFiles API called successfully. Returned data: ' + JSON.stringify(data));
      pag_files = data.values;
      for (var i = 0; i < pag_files.length; i++){
        var file_data = pag_files[i];
        var url = file_data.links.contentUrl; 
        if(file_data.kind !== 'Transcription'){
          continue;
        } 
        else {
          request(url, { json: true }, function(err, res, body) {
            if (err) { 
              return console.log(err); 
            }    
            else {
              console.log("\n Transcription File Content is : " + JSON.stringify(body));
            }// end else
          });// end request   
        }// end else
     }// end for
    }// end else
  };// end callbackGetTransFiles

  // invoke transcribe function to call batch transcription api
  _transcribe();

  // uncomment as needed to delete existing transcriptions
  // _deleteAllTranscriptions();

  function _transcribe(){
      
    console.log(DEFAULTPATH);
    // Configure API key authorization: apiKeyHeader
    var apiKeyHeader = defaultClient.authentications['apiKeyHeader'];
    apiKeyHeader.apiKey = API_KEY;
    // Configure API key authorization: apiKeyQuery
    var apiKeyQuery = defaultClient.authentications['apiKeyQuery'];
    apiKeyQuery.apiKey = API_KEY;

    defaultClient.basePath = DEFAULTPATH;

    // transcribe one file. - only one method of transcription can be used- _transcribeFromSingleBlob or _transcribeFromContainer
    var opts = _transcribeFromSingleBlob(RECORDINGS_BLOB_URI);

    // Uncomment this block to transcribe all files from a container. Comment the previous block - _transcribeFromSingleBlob when this is uncommented
    // var opts = _transcribeFromContainer(RECORDINGS_CONTAINER_URI);
      
    apiInstance.createTranscription(opts, function(error, data, response) {
      if (error) {
        console.error(error);
      } 
      else {
        var completed = false;
        var tid = data.self.substring(data.self.lastIndexOf("/")+1);
        console.log("Transcription ID created is : "+tid);
        apiInstance.getTranscription(tid,callbackGetTrans);
      }// end else
    });// end createTranscription   
  }// end of _transcribe()


  // GET transcriptions API returns transcription as pages (of 100 entities by default). 
  // Call this function multiple times; or implement the paged call.
  function _deleteAllTranscriptions(){    
    // Delete all transcriptions associated with your speech resource.
    console.log("Deleting all existing completed transcriptions.")
    // Configure API key authorization: apiKeyHeader
    var apiKeyHeader = defaultClient.authentications['apiKeyHeader'];
    apiKeyHeader.apiKey = API_KEY;
    // Configure API key authorization: apiKeyQuery
    var apiKeyQuery = defaultClient.authentications['apiKeyQuery'];
    apiKeyQuery.apiKey = API_KEY;
    
    defaultClient.basePath = DEFAULTPATH;

    // get all transcriptions for the subscription
    var opts = { 
    };
    var callback = function(error, data, response) {
      if (error) {
        console.error(error);
      } 
      else {
        // uncomment to display the response of Delete All transcriptions
        // console.log('Delete all transcriptions API called successfully. Returned data: ' + JSON.stringify(data));
        pag_files = data.values;
        console.log(" Total number of transcriptions : " + pag_files.length);
        for (var i = 0; i < pag_files.length; i++){
          var file_data = pag_files[i];
          var tid = file_data.self.substring(file_data.self.lastIndexOf("/")+1);
          if(file_data.status=='Succeeded' || file_data.status == 'Failed'){
            console.log("Deleting Transcription with status failed or success " + tid);
            var callback = function(error, data, response) {
              if (error) {
                console.error(error);
              } 
              else {
                console.log('Transcription deleted - API called successfully.'+tid);
              }//end else
            };//end callback
            //api to delete the transcription 
            apiInstance.deleteTranscription(tid, callback);
          }//end if
          else{
            console.log("Status running" + file_data.status);
          }
        }//end for
      };//end else
    }// end callback
    apiInstance.getTranscriptions(opts, callback);
  }// end deleteAllTranscriptions

  function _getTranscriptionFiles(){
    var opts = { 
    // Transcription | The details of the new transcription.
      'transcription' : new SpeechToTextApiV30.Transcription()
    }//end opts
    return opts;
  }

  function _transcribeFromSingleBlob(uri){
    var opts = { 
      // Transcription | The details of the new transcription.
      'transcription' : new SpeechToTextApiV30.Transcription()
    }  
    opts.transcription = {
      "contentUrls": [uri],
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
    opts.transcription = {
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


  function _checkComplete(tid){
    var status ; 
    var completed = false;
    while(completed!=true){
      // wait for 5 seconds before refreshing the transcription status
      sleep(5000);
      apiInstance.getTranscription(tid, function(error, data, response){
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
      });
    }// end while
    return status; 
  }
  
}());
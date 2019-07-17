# Test UniMRCP server and plugins with FreeSWITCH

## Install and run FreeSWITCH

See the official installation guide for details.
Before installation, edit `modules.conf` to ensure `languages/mod_lua` and `asr_tts/mod_unimrcp` are uncommented.

The FreeSWITCH is installed in `/usr/local/freeswitch/` by default, which is referred as `FS_ROOT` below.

## Add configure files

### unimrcpserver-mrcp-v2.xml

Add or edit `unimrcpserver-mrcp-v2.xml` in `FS_ROOT/conf/mrcp_profiles`.
  
  ```xml  
    <include>
        <profile name="unimrcpserver-mrcp2" version="2">
        <param name="client-ip" value="client_ip"/>
        <param name="client-port" value="5069"/>
        <param name="server-ip" value="server_ip"/>
        <param name="server-port" value="8060"/>
        <param name="sip-transport" value="udp"/>
        <param name="rtp-ip" value="client_ip"/>
        <param name="rtp-port-min" value="4000"/>
        <param name="rtp-port-max" value="5000"/>
        <param name="codecs" value="PCMU PCMA L16/96/8000"/>
        <synthparams>
        </synthparams>
        <recogparams>
        </recogparams>
        </profile>
    </include>
  ```

See `MRCPv2 example` in [mod_unimrcp documentation](https://freeswitch.org/confluence/display/FREESWITCH/mod_unimrcp) for details.

### autoload_configs/unimrcp.conf.xml

Edit `unimrcp.conf.xml` in `FS_ROOT/conf/autoload_configs`. Change the value of `default-tts-profile` and `default-asr-profile` to `unimrcpserver-mrcp2`.

  ```xml  
<configuration name="unimrcp.conf" description="UniMRCP Client">
  <settings>
    <!-- UniMRCP profile to use for TTS -->
    <param name="default-tts-profile" value="unimrcpserver-mrcp2"/>
    <!-- UniMRCP profile to use for ASR -->
    <param name="default-asr-profile" value="unimrcpserver-mrcp2"/>
    <!-- UniMRCP logging level to appear in freeswitch.log.  Options are:
         EMERGENCY|ALERT|CRITICAL|ERROR|WARNING|NOTICE|INFO|DEBUG -->
    <param name="log-level" value="DEBUG"/>
    <!-- Enable events for profile creation, open, and close -->
    <param name="enable-profile-events" value="false"/>

    <param name="max-connection-count" value="100"/>
    <param name="offer-new-connection" value="1"/>
    <param name="request-timeout" value="3000"/>
  </settings>

  <profiles>
    <X-PRE-PROCESS cmd="include" data="../mrcp_profiles/*.xml"/>
  </profiles>

</configuration>
  ```

### Dailplan

Add `02_unimrcp_test.xml` in `FS_ROOT/conf/dialplan/default`.

  ```xml  
<extension name="unimrcp">
    <condition field="destination_number" expression="^5011$">
        <action application="answer"/>
        <action application="lua" data="mrcp_test.lua"/>
    </condition>
</extension>
  ```

### Answer scripts

Add `mrcp_test.lua` in `FS_ROOT/scripts`.

  ```lua
  session:answer()
  menu = "ivr/ivr-this_ivr_will_let_you_test_features.wav"
  session:execute("play_and_detect_speech",menu .. "detect:unimrcp {start-input-timers=false}builtin:grammar/phone" )
  xml = session:getVariable('detect_speech_result')
  if (xml == nil) then
    freeswitch.consoleLog("ERROR","No recognition result.\n")
  else
    freeswitch.consoleLog("NOTICE","Result is '" .. xml .. "'\n")
  end

  session:sleep(250)
  session:set_tts_params("unimrcp", "");
  session:speak("today is a nice day");
  session:hangup()
  ```

## Run test

* start FreeSwitch, then type `load mod_unimrcp` in FreeSWITCH console to load MRCP module.
* Use a SIP client (eg. [X-Lite](https://www.counterpath.com/x-lite/)) to log in the FreeSWITCH and dial `5011` for test.

## Reference

* [FreeSWITCH MRCP module](https://freeswitch.org/confluence/display/FREESWITCH/mod_unimrcp)
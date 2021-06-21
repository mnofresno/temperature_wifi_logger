const char CONFIG_HTML[] = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>ESP Sensor Config Page</title>
  </head>
  <body>
    <h3>Please set this device's config</h3>
    <script src='core.js'></script>
    <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
    <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
      <h4>Connection config</h4>
      <input type='text' placeholder='Wifi SSID' name='wifiSSID'>
      <input type='text' placeholder='Wifi Password' name='wifiKey'>
      <h4>Webserver config</h4>
      <input type='text' placeholder='Admin username' name='wwwUsername'>
      <input type='text' placeholder='Admin password' name='wwwPassword'>
      <h4>Sensor config</h4>
      <input type='text' placeholder='Target url, e.g.: api.thingspeak.com' name='targetHost'>
      <input type='text' placeholder='API key query parameter name' name='apiKeyFieldName'>
      <input type='text' placeholder='API key' name='apiKey'>
      <input type='number' placeholder='Reporting seconds interval' name='reportingIntervalSeconds'>
      <input id='btnSubmit' type='submit' value='Update'>
    </form>
  </body>
</html>
)=====";
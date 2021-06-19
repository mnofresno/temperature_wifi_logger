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
      <input type='text' placeholder='Wifi SSID' name='ssid'>
      <input type='text' placeholder='Wifi Password' name='password'>
      <h4>Sensor config</h4>
      <input type='text' placeholder='Target url https://server...' name='targetUrl'>
      <input type='text' placeholder='API key query parameter name' name='paramName'>
      <input type='text' placeholder='API key' name='apiKey'>
      <input type='number' placeholder='Reporting seconds interval' name='interval'>
      <input id='btnSubmit' type='submit' value='Update'>
    </form>
    <div id='prg'>progress: 0%</div>
  </body>
</html>
)=====";
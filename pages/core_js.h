const char CORE_JS[] = R"=====(
function show_reload_warning() {
  $('#prg').html('Finished. Please wait 5 seconds while the device is rebooting...');
}
$('form').submit(function(e){
  $('#btnSubmit').attr('style', 'display: none;');
  $('input[name=\update\]').attr('style', 'display: none;');
  e.preventDefault();
  var form = $('#upload_form')[0];
  var data = new FormData(form);
  $.ajax({
    url: '/update',
    type: 'POST',
    data: data,
    contentType: false,
    processData:false,
    xhr: function() {
      var xhr = new window.XMLHttpRequest();
      xhr.upload.addEventListener('progress', function(evt) {
        if (evt.lengthComputable) {
        var per = evt.loaded / evt.total;
          $('#prg').html('progress: ' + Math.round(per*100) + '%');
        }
      }, false);
      return xhr;
    },
    success:function(d, s) {
      console.log('success!');
      show_reload_warning();
      setTimeout(function() {
        window.location.reload();
      }, 5000);
    },
    error: function (a, b, c) {
      console.log('Error uploading file');
      alert('There was an error uploading the firmware. Please try again!');
    }
  });
});
)=====";
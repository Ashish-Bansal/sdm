var sdmDownloadDiv = document.createElement('div');
sdmDownloadDiv.className = 'sdm_video_div';
sdmDownloadDiv.innerHTML = 'Download Video with SDM &nbsp;<span class="sdm_close">&#x2717;</span>';

var videoElements = document.getElementsByTagName('video');
for (var i = 0; i < videoElements.length; i++) {
    var parent = videoElements[i].parentNode;
    parent.appendChild(sdmDownloadDiv);
}

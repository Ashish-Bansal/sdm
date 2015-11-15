var map = (function() {
    var _start = 0;
    var _end = _maxLength;
    var _maxLength = 200;

    var _map = Object.create(null);
    var _keys = [];

    function map() {
        this.len = function() {
            return _keys.length;
        }

        this.setMaxLength = function(len) {
            _maxLength = len;
        }

        this.getMaxLength = function() {
            return _maxLength;
        }

        this.insert = function(key, value) {
            if (this.len.apply() == this.getMaxLength.apply()
                && typeof _map[key] == "undefined") {
                var id = _keys.shift();
                delete _map[id];
            }

            _map[key] = value;
            _keys.push(key);
        };

        this.value = function(key) {
            return _map[key];
        };

        this.contains = function(key) {
            return typeof _map[key] != "undefined"
        };
    }

    return map;

})();

var requests = new map();

var url = "ws://127.0.0.1:33533";
var socket = new WebSocket(url);

socket.onopen = function() {
    console.log('Web Socket open');
};

socket.onerror = function (error) {
    console.error('Web Socket error');
};

chrome.webRequest.onBeforeSendHeaders.addListener(
    function(details) {
        requests.insert(details.requestId, details);
        return {requestHeaders: details.requestHeaders};
    },
    {urls: ["<all_urls>"]},
    ["requestHeaders"]
);

chrome.webRequest.onHeadersReceived.addListener(
    function(details) {
        var contentType = "";
        var contentDisposition = "";
        for (var i = 0; i < details.responseHeaders.length; ++i) {
            if (details.responseHeaders[i].name.toLowerCase() === 'content-type') {
                contentType = details.responseHeaders[i].value;
            } else if (details.responseHeaders[i].name.toLowerCase() === "content-disposition") {
                contentDisposition = details.responseHeaders[i].value;
            }
        }

        if (contentDisposition.indexOf("attachment") > -1 && contentType.indexOf("application") > -1) {
            if (socket.readyState == 1) {
                details.responseHeaders.redirectUrl = "http://127.0.0.1";
                if (requests.contains(details.requestId)) {
                    var headers = requests.value(details.requestId);
                    var jsonValue = JSON.stringify(headers);
                    socket.send(jsonValue);
                    return {redirectUrl: "javascript:"};
                }
            }
        }

        if (contentType.indexOf("video") > -1) {
            console.log(details.url);
            //ToDo: Add an option to download that file.
        }

        return {responseHeaders: details.responseHeaders};
    },
    {urls: ["<all_urls>"]},
    ["blocking", "responseHeaders"]
);

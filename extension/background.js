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
    }

    return map;

})();

var requests = new map();

chrome.webRequest.onBeforeSendHeaders.addListener(
    function(details) {
        requests.insert(details.id, details);
        return {requestHeaders: details.requestHeaders};
    },
    {urls: ["<all_urls>"]},
    ["requestHeaders"]
);

chrome.webRequest.onHeadersReceived.addListener(
    function(details) {
        for (var i = 0; i < details.responseHeaders.length; ++i) {
            if (details.responseHeaders[i].name.toLowerCase() === 'content-type'
                && details.responseHeaders[i].value.indexOf("video") > -1) {
                console.log(details);
                console.log(details.url);
            }
        }
        return {responseHeaders: details.responseHeaders};
    },
    {urls: ["<all_urls>"]},
    ["responseHeaders"]
);

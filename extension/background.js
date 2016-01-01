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
var videoRequests = new map();

var url = "ws://127.0.0.1:33533";

var socket = new WebSocket(url);

setInterval(function() {
    if (socket.readyState != 1) {
        socket = new WebSocket(url);
    }
}, 3000);

function removeURLParameters(url, parameters) {
    parameters.forEach(function(parameter) {
        var urlparts = url.split('?');
        if (urlparts.length >= 2) {
            var prefix = encodeURIComponent(parameter) + '=';
            var pars = urlparts[1].split(/[&;]/g);

            for (var i = pars.length; i-- > 0;) {
                if (pars[i].lastIndexOf(prefix, 0) !== -1) {
                    pars.splice(i, 1);
                }
            }

            url = urlparts[0] + '?' + pars.join('&');
        }
    });
    return url;
}

chrome.webRequest.onSendHeaders.addListener(
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
                    if (jsonValue.indexOf("xmlhttprequest") == -1
                        && jsonValue.indexOf("script") == -1) {
                        socket.send(jsonValue);
                        return {redirectUrl: "javascript:"};
                    }
                }
            }
        }

        if (contentType.indexOf("video") > -1) {
            if (requests.contains(details.requestId)) {
                videoRequests.insert(details.tabId, requests.value(details.requestId));
            }
        }

        return {responseHeaders: details.responseHeaders};
    },
    {urls: ["<all_urls>"]},
    ["blocking", "responseHeaders"]
);

chrome.tabs.onActivated.addListener(function(tab) {
    if (videoRequests.contains(tab.tabId)) {
        chrome.browserAction.setIcon({
            path : {
                38 : "img/videoAvailable128.png"
            }
        });
    } else {
        chrome.browserAction.setIcon({
            path : {
                38 : "img/icon128.png",
            }
        });
    }
});

chrome.browserAction.onClicked.addListener(function(tab) {
    if (videoRequests.contains(tab.id)) {
        if (socket.readyState == 1) {
            var request = videoRequests.value(tab.id);
            var parametersToBeRemoved = ['range', 'rn', 'rbuf'];
            var url = removeURLParameters(request.url, parametersToBeRemoved);
            request.url = url;
            var jsonValue = JSON.stringify(request);
            socket.send(jsonValue);
        }
    }
});

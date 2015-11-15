chrome.webRequest.onHeadersReceived.addListener(
    function(details) {
        for (var i = 0; i < details.responseHeaders.length; ++i) {
            if (details.responseHeaders[i].name.toLowerCase() === 'content-type'
                && details.responseHeaders[i].value.indexOf("video") > -1) {
                console.log(details);
                console.log(details.url);
            }
        }
        return {requestHeaders: details.requestHeaders};
    },
    {urls: ["<all_urls>"]},
    ["responseHeaders"]
);

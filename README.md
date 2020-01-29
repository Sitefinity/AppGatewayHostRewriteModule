# AppGatewayHostRewriteModule 1.0.1 (App Service extension)

# Installation
Install this extension using your App Service's SCM site.

# Description
Once installed, you will need to restart site and SCM site (through kudu). Afterwards, all requests will be rewritten so the Host header matches the X-Original-Host.

This extension limits itself to the App Service Slot it is being installed through. The exension аdds to the App Service IIS a global native C++ module which replaces the HOST header value with the value of X-Original-Host header in case it exists.

# Notes
There is an approach to rewrite the HOST header with the value of X-Original-Host header by firstly unlocking the Host and X-Original-Host headers so they can be rewritten using rewrite rules. And then once the request reaches the App Service, the value of HOST header is set to the value of X-Original-Host header.
However this approach has one general pitfall, if there are other rewrite rules executed after the rule for rewriting the HOST header, they do not respect the new value of HOST header.
Using the native module, ensures that HOST header has taken the value of X-Original-Host header before the rewrite module steps in.

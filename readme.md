noCheat
=======
noCheat is a new approach on the constant defense against cheaters/hackers/botters in online games.

noCheat aims to create a web-based API in which game developers/players can interface to update/query a player's games, play times, statistics, videos/screenshots, and most importantly, their gaming legitimacy.

This repository contains the whole ordeal - both the Windows go-along application, as well as the web API.

**This repository will be set to private whenever I get the funds to do so. THIS IS NOT A PUBLIC PROJECT!**

*This project is led/programmed by Qix*


A Quick Run-Through
-------------------
When discerning between the various components of the noCheat system, refer to each as the following:
* noCheat - *The noCheat desktop application, which is the end-user's interface to the noCheat system (other than the website)
* noCheat**S** - *The noCheat service - basically, the behind-the-scenes processing that actually performs the checks on processes, etc. The service, for some functionality, is the middleman between the desktop application and the web, and performs all of the checks/reporting.
* noCheat**W** or noCheat**Web** - The web-portion of noCheat. This includes all PHP scripts and front-end design.
* noCheat**D** - The noCheat Driver, which feeds the service information that cannot be [safely] obtained by the user-land components.

![noCheat Component Diagram](https://github.com/Qix-/noCheat/raw/master/Resources/ComponentDiagram.png)

These terms will be scattered around documentation so make sure you understand wtf I'm talking about

Quick References for Programmers
--------------------------------
* http://msdn.microsoft.com/en-us/library/windows/desktop/aa394372(v=vs.85).aspx - **Gives process information to be read when a process starts (for use in the ncWBEMResult class)**


Web API Information
-------------------
Each API url will be hardcoded (encrypted) into the service itself, and will communicate with noCheat

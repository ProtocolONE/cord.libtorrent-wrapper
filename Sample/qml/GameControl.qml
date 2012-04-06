/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

import QtQuick 1.0
import "Elements" as Elements

Item {

	property string gameId:  "0000011"

	property string statusText:  "status text "
	property int progressValue: 0
	id: gameControl

//    signal startClicked();
//    signal stopClicked();
//    signal restartClicked();

    Elements.Button2 {
        x: gameControl.x
        y: gameControl.y
	buttonText: "start"
	onButtonClicked: {
		//console.log("start " + gameId);
		mainWindow.startTorrent(gameId);
	}
    }

    Elements.Button2 {
        x: gameControl.x + 60
        y: gameControl.y
	buttonText: "stop"
	onButtonClicked:{
		mainWindow.stopTorrent(gameId);
	}
    }

    Elements.Button2 {
        x: gameControl.x + 120
        y: gameControl.y
	buttonText: "replace"
	onButtonClicked:{
		mainWindow.restartTorrent(gameId);
	}
    }


    Elements.ProgressBar3{
        id: progressBar
        x: gameControl.x + 200
        y: gameControl.y
	width: 300
	color: "red"
        value: progressValue
    }

    Text {
        x: gameControl.x
        y: gameControl.y + 30
        text:       statusText
    }


}
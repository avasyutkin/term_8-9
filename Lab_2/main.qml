import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.1
import SortFilterProxyModel 0.2
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id: window
    width: 400
    height: 250
    visible: true

    ListModel {
        id: accountModel
    }

    ListModel {
        id: backupModel
    }

    Connections {
        target: Backend
        onSend_backup_to_qml: {
            for (var i in count)
                backupModel.append({"backup":count[i]})
            console.log(backupModel.rowCount())
        }
        onSend_sites_to_qml: {
            for (var i in count)
                accountModel.append({"url":count[i]})
        }
    }


    Page {
        id: window1
        width: 400
        height: 250
        visible: true

        TextField {
            id: text_pin
            width: 140

            placeholderText: qsTr("Введите пин-код")
            font.family: "SF UI Display Light"
            font.pixelSize: 14

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 130
            echoMode: TextInput.Password

            onTextChanged: if (length > 4) remove(4, length)
        }

        Button{
            id: button_login
            width: 140

            text: "Войти"
            font.family: "SF UI Display Light"
            font.pixelSize: 16

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80

            enabled: if (text_pin.length < 4) false
                     else true

            onClicked:
                if (Backend.get_pass(text_pin.text) == 0) {
                    popup_login.open();
                    text_pin.clear();
                }
                else {
                    window2.visible = true;
                    window1.visible = false
                    Backend.get_sites()
                    text_pin.clear()
                }
        }

        Popup {
            id: popup_login
            width: 200
            height: 100

            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)

            closePolicy: CloseOnPressOutsideParent

            background: Rectangle {
                anchors.fill: popup_login
                color: "#FF0000" }
            Label {
                text: qsTr("Неверный пин-код")
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 16
                font.bold: true }
        }
    }

    Page{
        id: window2
        width: 400
        height: 250
        visible: false

        SortFilterProxyModel {
            id: accountProxyModel
            sourceModel: accountModel
            filters: RegExpFilter {
                roleName: "url"
                pattern: search_text.text
            }
        }

        ListView {
            id: account_list
            anchors.fill: parent
            topMargin: 40
            model: accountProxyModel
            spacing: 10

            Rectangle {
                x: 10
                width: 380
                height: 30
                TextField {
                    id: search_text
                    width: 255
                    height: 30
                    font.family: "SF UI Display Light"
                    placeholderText: qsTr("Найти...")
                    font.pixelSize: 14
                    focus: true

                }
                Button{
                    id: button_add_new
                    height: 30
                    width: 30
                    anchors.left: search_text.right
                    anchors.leftMargin: 10
                    text: "+"
                    font.family: "SF UI Display Light"
                    font.pixelSize: 16
                    onClicked: {
                        if (Backend.get_new_account_bool(search_text.text) == true){
                            accountModel.clear();
                            Backend.get_new_account(search_text.text);
                            search_text.text = ""
                        }
                        else {
                            ToolTip.show("Введите в поле три значения через пробел: название ресурса, логин и пароль", 5000)
                            search_text.text = ""
                        }
                    }
                }
                Button{
                    id: button_backup_list
                    height: 30
                    width: 30
                    anchors.left: search_text.right
                    anchors.leftMargin: 50
                    text: "↑"
                    font.family: "SF UI Display Light"
                    font.pixelSize: 16
                    onClicked: {
                        window2.visible = false
                        page_list_backup.visible = true
                        Backend.get_backup_files()
                        search_text.text = ""
                    }
                }

                Button{
                    id: button_to_print
                    height: 30
                    width: 30
                    anchors.left: search_text.right
                    anchors.leftMargin: 90
                    text: "↓"
                    font.family: "SF UI Display Light"
                    font.pixelSize: 16
                    onClicked: {
                        Backend.print_all_data()
                    }
                }

            }

            delegate: Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 20

                Button{
                    width: 30
                    height: 30
                    Image {
                        width: 30
                        height: 30
                        fillMode: Image.PreserveAspectFit
                        source: "file:///C:/Users/Alexander/Desktop/stud/Lab_2/logovk.png"
                    }
                    onDoubleClicked: {
                        deleteDialog.visible = true
                    }
                }
                Text {
                    text: url
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "SF UI Display Light"
                    font.pixelSize: 14
                }
                TextField {
                    text: url
                    echoMode: TextInput.Password
                    width: 100
                    height: 30
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "SF UI Display Light"
                    font.pixelSize: 14
                    readOnly: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            ToolTip.show("Скопировано", 550)
                            Backend.send_site_to_cpp(0, parent.text)
                        }
                    }

                }
                TextField {
                    text: url
                    echoMode: TextInput.Password
                    width: 100
                    height: 30
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "SF UI Display Light"
                    font.pixelSize: 14
                    readOnly: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            ToolTip.show("Скопировано", 550)
                            Backend.send_site_to_cpp(1, parent.text)
                        }
                    }
                }

                MessageDialog {
                    id: deleteDialog
                    title: "Удаление аккаунта"
                    text: "Действительно хотите удалить аккаунт?"
                    icon: StandardIcon.Warning
                    standardButtons: StandardButton.Yes | StandardButton.No
                    onYes: {
                        Backend.delete_account(url)
                        accountModel.clear(url)
                        Backend.upd_model_qml()
                    }
                }
            }
        }
    }
    Page {
        id:page_list_backup
        visible: false
        ListView {
            model: backupModel
            anchors.top: parent.top
            anchors.topMargin: 10
            height: 230
            spacing: 10
            width: 400
            delegate: Button {
                id: radio_delegate
                width: 260
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: backup
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "SF UI Display Light"
                    font.pixelSize: 14
                }
                onClicked: {
                    Backend.get_name_file(backup)
                    page_list_backup.visible = false
                    accountModel.clear()
                    backupModel.clear()
                    window1.visible = true
                }
            }
            Rectangle{
                width: parent.width
                height: 40
                anchors.bottom: parent.bottom
                color: "white"
                Button {
                    id: button_back
                    height: 30
                    width: 50
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter

                    onClicked: {
                        window2.visible = true
                        page_list_backup.visible = false
                        backupModel.clear()
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Назад"
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "SF UI Display Light"
                        font.pixelSize: 14
                    }
                }
            }
        }
    }
}


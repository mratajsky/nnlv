#
# NNLV project file
#
QT      += core gui widgets charts xml xmlpatterns

CONFIG  += c++11
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

TARGET      = NNLV
TEMPLATE    = app
RC_FILE     = NNLV.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
        mainwindow.cpp \
        adalinebiasneuron.cpp \
        adalinecreatenetworkwidget.cpp \
        adalineinputlayer.cpp \
        adalineinputneuron.cpp \
        adalinenetwork.cpp \
        adalinenetworkwizardpage.cpp \
        adalineoutputlayer.cpp \
        adalineoutputneuron.cpp \
        adalinetrainingoptionsdialog.cpp \
        adalineviewwidget.cpp \
        colors.cpp \
        csvworker.cpp \
        custominputdialog.cpp \
        graphicsutilities.cpp \
        helpbrowser.cpp \
        iconlabel.cpp \
        kmeansclustering.cpp \
        kohonencreatenetworkwidget.cpp \
        kohoneninputlayer.cpp \
        kohoneninputneuron.cpp \
        kohonenlayer.cpp \
        kohonennetwork.cpp \
        kohonennetworkwizardpage.cpp \
        kohonenoutputlayer.cpp \
        kohonenoutputneuron.cpp \
        kohonentrainingoptionsdialog.cpp \
        kohonenviewwidget.cpp \
        kohonenweightchartview.cpp \
        kohonenweightchartwidget.cpp \
        maindockwidget.cpp \
        mlpbiasneuron.cpp \
        mlpcreatenetworkwidget.cpp \
        mlphiddenlayer.cpp \
        mlphiddenneuron.cpp \
        mlpinputlayer.cpp \
        mlpinputneuron.cpp \
        mlplayer.cpp \
        mlpnetwork.cpp \
        mlpnetworkwizardpage.cpp \
        mlpneuron.cpp \
        mlpoutputlayer.cpp \
        mlpoutputneuron.cpp \
        mlptrainingoptionsdialog.cpp \
        mlpviewwidget.cpp \
        networkchartview.cpp \
        networkconnection.cpp \
        network.cpp \
        networkexamplelistitemwidget.cpp \
        networkinteractivechartview.cpp \
        networklayer.cpp \
        networklayerdialog.cpp \
        networklayoutwidget.cpp \
        networkneuron.cpp \
        networkneurondialog.cpp \
        networkneurondialogspinbox.cpp \
        networkstatusnumberlabel.cpp \
        networkstatuswidget.cpp \
        networkviewwidget.cpp \
        networkvisualwidget.cpp \
        networkwizard.cpp \
        networkwizardmainpage.cpp \
        networkwizardpage.cpp \
        optionsdialog.cpp \
        rbfbiasneuron.cpp \
        rbfcreatenetworkwidget.cpp \
        rbfhiddenlayer.cpp \
        rbfhiddenneuron.cpp \
        rbfinputlayer.cpp \
        rbfinputneuron.cpp \
        rbflayer.cpp \
        rbfnetwork.cpp \
        rbfnetworkneurondialog.cpp \
        rbfnetworkwizardpage.cpp \
        rbfneuron.cpp \
        rbfoutputlayer.cpp \
        rbfoutputneuron.cpp \
        rbftrainingoptionsdialog.cpp \
        rbfviewwidget.cpp \
        rbfweightchartview.cpp \
        rbfweightchartwidget.cpp \
        renamenetworkdialog.cpp \
        resetweightsdialog.cpp \
        savednetwork.cpp \
        savednetworklayer.cpp \
        savednetworkneuron.cpp \
        slpbiasneuron.cpp \
        slpcreatenetworkwidget.cpp \
        slpinputlayer.cpp \
        slpinputneuron.cpp \
        slplayer.cpp \
        slpnetwork.cpp \
        slpnetworkwizardpage.cpp \
        slpneuron.cpp \
        slpoutputlayer.cpp \
        slpoutputneuron.cpp \
        slptrainingoptionsdialog.cpp \
        slpviewwidget.cpp \
        supervisedchart.cpp \
        supervisedchartview.cpp \
        supervisedchartwidget.cpp \
        supervisederrorchartview.cpp \
        supervisederrorchartwidget.cpp \
        supervisedlayer.cpp \
        supervisednetwork.cpp \
        trainingsample.cpp \
        trainingsamplestore.cpp \
        trainingtabledialog.cpp \
        trainingtablemodel.cpp \
        utilities.cpp \
        vectorutilities.cpp \
        xmlmessagehandler.cpp \
        xmlworker.cpp

HEADERS += mainwindow.h \
        adalinebiasneuron.h \
        adalinecreatenetworkwidget.h \
        adalineinputlayer.h \
        adalineinputneuron.h \
        adalinenetwork.h \
        adalinenetworklimits.h \
        adalinenetworkwizardpage.h \
        adalineoutputlayer.h \
        adalineoutputneuron.h \
        adalinetrainingoptionsdialog.h \
        adalineviewwidget.h \
        colors.h \
        common.h \
        csvworker.h \
        custominputdialog.h \
        graphicsutilities.h \
        helpbrowser.h \
        iconlabel.h \
        kmeansclustering.h \
        kohonencreatenetworkwidget.h \
        kohoneninputlayer.h \
        kohoneninputneuron.h \
        kohonenlayer.h \
        kohonennetworkdefaults.h \
        kohonennetwork.h \
        kohonennetworklimits.h \
        kohonennetworkwizardpage.h \
        kohonenoutputlayer.h \
        kohonenoutputneuron.h \
        kohonentrainingoptionsdialog.h \
        kohonenviewwidget.h \
        kohonenweightchartview.h \
        kohonenweightchartwidget.h \
        maindockwidget.h \
        mlpactivation.h \
        mlpbiasneuron.h \
        mlpcreatenetworkwidget.h \
        mlphiddenlayer.h \
        mlphiddenneuron.h \
        mlpinputlayer.h \
        mlpinputneuron.h \
        mlplayer.h \
        mlpnetwork.h \
        mlpnetworklimits.h \
        mlpnetworkwizardpage.h \
        mlpneuron.h \
        mlpoutputlayer.h \
        mlpoutputneuron.h \
        mlptrainingoptionsdialog.h \
        mlpviewwidget.h \
        networkchartview.h \
        networkconnection.h \
        networkdefaults.h \
        networkexamplelistitemwidget.h \
        network.h \
        networkinfo.h \
        networkinteractivechartview.h \
        networklayerdialog.h \
        networklayer.h \
        networklayerinfo.h \
        networklayoutwidget.h \
        networklimits.h \
        networkneurondialog.h \
        networkneurondialogspinbox.h \
        networkneuron.h \
        networkneuroninfo.h \
        networkstatusnumberlabel.h \
        networkstatuswidget.h \
        networkviewwidget.h \
        networkvisualwidget.h \
        networkwizard.h \
        networkwizardmainpage.h \
        networkwizardpage.h \
        optionsdialog.h \
        program.h \
        rbfbiasneuron.h \
        rbfcreatenetworkwidget.h \
        rbfhiddenlayer.h \
        rbfhiddenneuron.h \
        rbfinputlayer.h \
        rbfinputneuron.h \
        rbflayer.h \
        rbfnetwork.h \
        rbfnetworklimits.h \
        rbfnetworkneurondialog.h \
        rbfnetworkwizardpage.h \
        rbfneuron.h \
        rbfoutputlayer.h \
        rbfoutputneuron.h \
        rbftrainingoptionsdialog.h \
        rbfviewwidget.h \
        rbfweightchartview.h \
        rbfweightchartwidget.h \
        renamenetworkdialog.h \
        resettable.h \
        resetweightsdialog.h \
        savednetwork.h \
        savednetworklayer.h \
        savednetworkneuron.h \
        slpbiasneuron.h \
        slpcreatenetworkwidget.h \
        slpinputlayer.h \
        slpinputneuron.h \
        slplayer.h \
        slpnetwork.h \
        slpnetworklimits.h \
        slpnetworkwizardpage.h \
        slpneuron.h \
        slpoutputlayer.h \
        slpoutputneuron.h \
        slptrainingoptionsdialog.h \
        slpviewwidget.h \
        supervisedchart.h \
        supervisedchartview.h \
        supervisedchartwidget.h \
        supervisederrorchartview.h \
        supervisederrorchartwidget.h \
        supervisedlayer.h \
        supervisednetwork.h \
        trainingsample.h \
        trainingsamplestore.h \
        trainingtabledialog.h \
        trainingtablemodel.h \
        utilities.h \
        vectorutilities.h \
        version.h \
        xmlmessagehandler.h \
        xmlworker.h

FORMS += mainwindow.ui \
        adalinecreatenetworkwidget.ui \
        adalinetrainingoptionsdialog.ui \
        createnetworkwidget.ui \
        custominputdialog.ui \
        helpbrowser.ui \
        kohonencreatenetworkwidget.ui \
        kohonennetworkwizardpage.ui \
        kohonentrainingoptionsdialog.ui \
        kohonenweightchartwidget.ui \
        mlpcreatenetworkwidget.ui \
        mlptrainingoptionsdialog.ui \
        networkexamplelistitemwidget.ui \
        networklayerdialog.ui \
        networklayoutwidget.ui \
        networkneurondialog.ui \
        networkstatuswidget.ui \
        networkviewwidget.ui \
        networkwizardmainpage.ui \
        networkwizard.ui \
        optionsdialog.ui \
        rbfcreatenetworkwidget.ui \
        rbfnetworkwizardpage.ui \
        rbftrainingoptionsdialog.ui \
        rbfweightchartwidget.ui \
        renamenetworkdialog.ui \
        resetweightsdialog.ui \
        slpcreatenetworkwidget.ui \
        slptrainingoptionsdialog.ui \
        supervisedchartwidget.ui \
        supervisederrorchartwidget.ui \
        trainingtabledialog.ui

RESOURCES += \
        examples.qrc \
        help.qrc \
        icons.qrc \
        resources.qrc

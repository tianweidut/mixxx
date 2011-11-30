// preparefeature.cpp
// Created 8/23/2009 by RJ Ryan (rryan@mit.edu)
// Forked 11/11/2009 by Albert Santoni (alberts@mixxx.org)

#include <QtDebug>

#include "library/preparefeature.h"
#include "library/librarytablemodel.h"
#include "library/trackcollection.h"
#include "dlgprepare.h"
#include "widget/wlibrary.h"
#include "widget/wlibrarysidebar.h"
#include "mixxxkeyboard.h"
#include "analyserqueue.h"

const QString PrepareFeature::m_sPrepareViewName = QString("Prepare");

PrepareFeature::PrepareFeature(QObject* parent,
                               ConfigObject<ConfigValue>* pConfig,
                               TrackCollection* pTrackCollection)
        : LibraryFeature(parent),
          m_pConfig(pConfig),
          m_pTrackCollection(pTrackCollection),
          m_pAnalyserQueue(NULL) {
}

PrepareFeature::~PrepareFeature() {
    // TODO(XXX) delete these
    //delete m_pLibraryTableModel;
    cleanupAnalyser();
}

QVariant PrepareFeature::title() {
    return tr("Analyze");
}

QIcon PrepareFeature::getIcon() {
    return QIcon(":/images/library/ic_library_prepare.png");
}

void PrepareFeature::bindWidget(WLibrarySidebar* sidebarWidget,
                                WLibrary* libraryWidget,
                                MixxxKeyboard* keyboard) {
    DlgPrepare* pPrepareView = new DlgPrepare(libraryWidget,
                                              m_pConfig,
                                              m_pTrackCollection);
    connect(pPrepareView, SIGNAL(loadTrack(TrackPointer)),
            this, SIGNAL(loadTrack(TrackPointer)));
    connect(pPrepareView, SIGNAL(loadTrackToPlayer(TrackPointer, QString)),
            this, SIGNAL(loadTrackToPlayer(TrackPointer, QString)));
    connect(pPrepareView, SIGNAL(analyzeTracks(QList<int>)),
            this, SLOT(analyzeTracks(QList<int>)));
    connect(pPrepareView, SIGNAL(stopAnalysis()),
            this, SLOT(stopAnalysis()));

    connect(this, SIGNAL(analysisActive(bool)),
            pPrepareView, SLOT(analysisActive(bool)));
    connect(this, SIGNAL(trackAnalysisProgress(TrackPointer, int)),
            pPrepareView, SLOT(trackAnalysisProgress(TrackPointer, int)));
    connect(this, SIGNAL(trackAnalysisFinished(TrackPointer)),
            pPrepareView, SLOT(trackAnalysisFinished(TrackPointer)));
    pPrepareView->installEventFilter(keyboard);

    // Let the DlgPrepare know whether or not analysis is active.
    bool bAnalysisActive = m_pAnalyserQueue != NULL;
    emit(analysisActive(bAnalysisActive));

    libraryWidget->registerView(m_sPrepareViewName, pPrepareView);
}

TreeItemModel* PrepareFeature::getChildModel() {
    return &m_childModel;
}

void PrepareFeature::activate() {
    //qDebug() << "PrepareFeature::activate()";
    emit(switchToView(m_sPrepareViewName));
}

void PrepareFeature::activateChild(const QModelIndex& index) {
}

void PrepareFeature::onRightClick(const QPoint& globalPos) {
}

void PrepareFeature::onRightClickChild(const QPoint& globalPos,
                                            QModelIndex index) {
}

bool PrepareFeature::dropAccept(QUrl url) {
    return false;
}

bool PrepareFeature::dropAcceptChild(const QModelIndex& index, QUrl url) {
    return false;
}

bool PrepareFeature::dragMoveAccept(QUrl url) {
    return false;
}

bool PrepareFeature::dragMoveAcceptChild(const QModelIndex& index,
                                              QUrl url) {
    return false;
}

void PrepareFeature::onLazyChildExpandation(const QModelIndex &index){
    //Nothing to do because the childmodel is not of lazy nature.
}

void PrepareFeature::analyzeTracks(QList<int> trackIds) {
    if (m_pAnalyserQueue == NULL) {
        //Save the old BPM detection prefs setting (on or off)
        m_iOldBpmEnabled = m_pConfig->getValueString(ConfigKey("[BPM]","BPMDetectionEnabled")).toInt();
        //Force BPM detection to be on.
        m_pConfig->set(ConfigKey("[BPM]","BPMDetectionEnabled"), ConfigValue(1));
        //Note: this sucks... we should refactor the prefs/analyser to fix this hacky bit ^^^^.

        m_pAnalyserQueue = AnalyserQueue::createPrepareViewAnalyserQueue(m_pConfig);

        connect(m_pAnalyserQueue, SIGNAL(trackProgress(TrackPointer, int)),
                this, SLOT(slotTrackAnalysisProgress(TrackPointer, int)));
        connect(m_pAnalyserQueue, SIGNAL(trackFinished(TrackPointer)),
                this, SLOT(slotTrackAnalysisFinished(TrackPointer)));
        connect(m_pAnalyserQueue, SIGNAL(queueEmpty()),
                this, SLOT(stopAnalysis()));
        emit(analysisActive(true));
    }

    foreach(int trackId, trackIds) {
        TrackPointer pTrack = m_pTrackCollection->getTrackDAO().getTrack(trackId);
        if (pTrack) {
            //qDebug() << this << "Queueing track for analysis" << pTrack->getLocation();
            m_pAnalyserQueue->queueAnalyseTrack(pTrack);
        }
    }
}

void PrepareFeature::slotTrackAnalysisProgress(TrackPointer pTrack, int progress) {
    //qDebug() << this << "trackAnalysisProgress" << pTrack->getInfo() << progress;
    emit(trackAnalysisProgress(pTrack, progress));
}

void PrepareFeature::slotTrackAnalysisFinished(TrackPointer pTrack) {
    //qDebug() << this << "trackAnalysisFinished" << pTrack->getInfo();
    emit(trackAnalysisFinished(pTrack));
}

void PrepareFeature::stopAnalysis() {
    //qDebug() << this << "stopAnalysis()";
    cleanupAnalyser();
    emit(analysisActive(false));
}

void PrepareFeature::cleanupAnalyser() {
    if (m_pAnalyserQueue != NULL) {
        delete m_pAnalyserQueue;
        m_pAnalyserQueue = NULL;
        //Restore old BPM detection setting for preferences...
        m_pConfig->set(ConfigKey("[BPM]","BPMDetectionEnabled"), ConfigValue(m_iOldBpmEnabled));
    }

}
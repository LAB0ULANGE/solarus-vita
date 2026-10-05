// SPDX-License-Identifier: GPL-3.0-or-later
#include <quests/QuestRunner.h>

#include <QCoreApplication>
#include <QByteArray>
#include <QStringList>

namespace solarus::launcher {

QuestRunner::QuestRunner(QObject* parent)
  : QObject(parent) {
  // Set the process channel mode to merged (stdout + stderr).
  _process.setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);

  // Connect to QProcess signals to know when the quest is running and finished.
  QObject::connect(&_process, &QProcess::errorOccurred, this, &QuestRunner::onProcessError);
  QObject::connect(&_process, &QProcess::readyReadStandardOutput, this, &QuestRunner::onProcessStandardOutputAvailable);
  QObject::connect(&_process, &QProcess::stateChanged, this, &QuestRunner::onProcessStateChanged);
  QObject::connect(
    &_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &QuestRunner::onProcessFinished);

  // Workaround to make the quest process close properly instead of hanging while reading on its stdin on Windows.
  constexpr auto timerDelay = 100; // ms
  QObject::connect(&_timer, &QTimer::timeout, this, &QuestRunner::onTimerTimeout);
  _timer.start(timerDelay);
}

QuestRunner::~QuestRunner() {
  if (_process.state() != QProcess::ProcessState::NotRunning) {
    // Give a chance to the quest process to finish properly.
    _process.terminate();
    constexpr auto waitDelay = 1000; // ms
    if (!_process.waitForFinished(waitDelay)) {
      // Kill it after a delay.
      _process.kill();
    }
  }
}

QuestRunner::State QuestRunner::state() const {
  switch (_process.state()) {
    case QProcess::ProcessState::NotRunning:
      return State::Stopped;
    case QProcess::ProcessState::Starting:
      return State::Starting;
    case QProcess::ProcessState::Running:
      return State::Running;
    default:
      return State::Stopped;
  }
}

const QString& QuestRunner::questFilePath() const {
  return _questFilePath;
}

const QString& QuestRunner::fullOutput() const {
  return _questFullOutput;
}

void QuestRunner::start(const QString& questFilePath) {
  if (questFilePath.isEmpty() && questFilePath != _questFilePath) {
    return;
  }

  if (_process.state() != QProcess::ProcessState::NotRunning) {
    return;
  }

  // Save the quest path.
  _questFullOutput.clear();
  _questFilePath = questFilePath;
  emit questChanged();

  // Prepare arguments for the command.
  const auto [programName, arguments] = createArguments(_questFilePath);

  // Run the current executable itself.
  _process.start(programName, arguments);
}

void QuestRunner::stop() {
  if (_process.state() != QProcess::ProcessState::NotRunning) {
    // Clear the previous quest.
    _questFullOutput.clear();
    _questFilePath.clear();
    emit questChanged();

    _process.terminate();
  }
}

int QuestRunner::executeCommand(const QString& command) {
  if (_process.state() != QProcess::ProcessState::NotRunning) {
    return -1;
  }

  if (command.trimmed().isEmpty()) {
    return -1;
  }

  auto commandUtf8 = command.toUtf8();
  commandUtf8.append('\n');
  const auto bytesWritten = _process.write(commandUtf8);
  if (bytesWritten != commandUtf8.size()) {
    return -1;
  }

  ++_lastCommandId;
  return _lastCommandId;
}

void QuestRunner::onProcessError(QProcess::ProcessError error) {
  switch (error) {
    case QProcess::ProcessError::FailedToStart:
      emit errorRaised(ErrorCode::ProcessFailedToStart);
      break;
    case QProcess::ProcessError::Crashed:
      emit errorRaised(ErrorCode::ProcessCrashed);
      break;
    case QProcess::ProcessError::Timedout:
      emit errorRaised(ErrorCode::ProcessTimedOut);
      break;
    case QProcess::ProcessError::WriteError:
      emit errorRaised(ErrorCode::ProcessWriteError);
      break;
    case QProcess::ProcessError::ReadError:
      emit errorRaised(ErrorCode::ProcessReadError);
      break;
    case QProcess::ProcessError::UnknownError:
      emit errorRaised(ErrorCode::UnknownError);
      break;
    default:
      break;
  }
}

void QuestRunner::onProcessFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/) {
  emit stateChanged();

  _lastCommandId = -1;

  // Clear the previous quest.
  _questFullOutput.clear();
  _questFilePath.clear();
  emit questChanged();

  emit stateChanged();
}

void QuestRunner::onProcessStandardOutputAvailable() {
  // Read the UTF-8 data available.
  QStringList lines;
  while (_process.canReadLine()) {
    auto line = QString::fromUtf8(_process.readLine());

    // Remove the final '\n'.
    line = line.trimmed();

    if (!line.isEmpty()) {
      lines << line;
    }
  }

  if (!lines.isEmpty()) {
    emit outputProduced(lines);
  }
}

void QuestRunner::onProcessStateChanged(QProcess::ProcessState state) {
  if (state == QProcess::ProcessState::NotRunning) {
    _lastCommandId = -1;
    // Clear the previous quest.
    _questFullOutput.clear();
    _questFilePath.clear();
    emit questChanged();
  }
  emit stateChanged();
}

void QuestRunner::onTimerTimeout() {
  if (_process.state() != QProcess::ProcessState::NotRunning) {
    _process.write(QByteArrayLiteral("\n"));
  }
}

std::pair<QString, QStringList> QuestRunner::createArguments(const QString& questPath) const {
  // Options should be first, then quest path as ultimate arg.
  const auto programName = QCoreApplication::applicationFilePath();
  QStringList arguments;

  if (!audioEnabled()) {
    arguments << QStringLiteral("-no-audio");
  }

  if (forceSoftwareRendering()) {
    arguments << QStringLiteral("-force-software-rendering");
  }

  if (fullScreen()) {
    arguments << QStringLiteral("-fullscreen");
  }

  if (!suspendWhenUnfocused()) {
    arguments << QStringLiteral("-suspend-unfocused=no");
  }

  arguments << questPath;

  return { programName, arguments };
}

bool QuestRunner::audioEnabled() const {
  return _audioEnabled;
}

void QuestRunner::setAudioEnabled(bool value) {
  _audioEnabled = value;
}

bool QuestRunner::forceSoftwareRendering() const {
  return _forceSoftwareRendering;
}

void QuestRunner::setForceSoftwareRendering(bool value) {
  _forceSoftwareRendering = value;
}

bool QuestRunner::fullScreen() const {
  return _fullScreen;
}

void QuestRunner::setFullScreen(bool value) {
  _fullScreen = value;
}

bool QuestRunner::suspendWhenUnfocused() const {
  return _suspendWhenUnfocused;
}

void QuestRunner::setSuspendWhenUnfocused(bool value) {
  _suspendWhenUnfocused = value;
}
} // namespace solarus::launcher

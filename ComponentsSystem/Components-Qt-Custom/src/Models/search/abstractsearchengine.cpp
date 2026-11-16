#include "abstractsearchengine.hpp"

#include <QModelIndex>

namespace QtCustom::Models {

AbstractSearchEngine::AbstractSearchEngine(QObject *parent)
    : QObject{parent}
{

}

bool AbstractSearchEngine::isWanted([[maybe_unused]] const QModelIndex &sourceIndex) const {
    return true;
}


void TextSearchEngine::setTarget(const QString &value)
{
    m_targetTrigrams = createTrigrams(value);
    m_indexDeltaPercent = 1.0 / double(m_targetTrigrams.size()) / 3.0;
    emit sig_filterChanged();
}

void TextSearchEngine::setThreshold(double searchThreshold)
{
    m_searchThreshold = searchThreshold;
    emit sig_filterChanged();
}

bool TextSearchEngine::isWanted(const QModelIndex &sourceIndex) const
{
    return containTarget(m_targetTrigrams, sourceIndex.data(Qt::DisplayRole).toString(), m_searchThreshold);
}

std::vector<TextSearchEngine::trigram_t> TextSearchEngine::createTrigrams(const QString &text) const {
    if (text.isEmpty())
        return {};

    std::vector<trigram_t> trigrams;
    trigrams.reserve(text.size() / 3);

    trigram_t currentTrigram;
    for (const auto& ch : text) {
        if (currentTrigram.size() < 3) {
            currentTrigram.insert(ch);
            continue;
        }
        trigrams.emplace_back(std::move(currentTrigram));
        currentTrigram.insert(ch);
    }

    if (!currentTrigram.empty()) {
        if (currentTrigram.size() < 3) {
            currentTrigram.insert(' ');
        }
        trigrams.emplace_back(std::move(currentTrigram));
    }
    return trigrams;
}

double TextSearchEngine::getSamePercent(const std::vector<trigram_t> &targetTrigram,
                                        const std::vector<trigram_t> &sourceTrigram,
                                        double threshold) const {
    double samePercent {};
    std::size_t targetIdx {};
    for (const auto& trig : targetTrigram) {
        if (sourceTrigram.size() <= targetIdx) { break; }
        const auto& sourceTrig = sourceTrigram[targetIdx];
        for (const auto& ch : trig) {
            samePercent += ((ch == ' ') | (sourceTrig.count(ch) != 0)) * m_indexDeltaPercent;
        }
        if (trig.size() == 2 && trig.count(' ')) { // Corner case
            samePercent += m_indexDeltaPercent;
        }
        if (samePercent > threshold) {
            break;
        }
        ++targetIdx;
    }
    return samePercent;
}

bool TextSearchEngine::containTarget(const std::vector<trigram_t> &targetTrigrams, const QString &sampleText, double threshold) const {
    double maxPercent {};
    for (const auto& tok : sampleText.split(' ')) {
        auto samePercent = getSamePercent(targetTrigrams, createTrigrams(tok), threshold);
        maxPercent = std::max(maxPercent, samePercent);
        if (maxPercent > threshold) {
            break;
        }
    }
    return (maxPercent > threshold);
}

} // namespace QtCustom::Search

#include "QCachingLocale.h"
#include "Parser.h"
#include "Analyst.h"
#include <QObject>
#include <QTime>

int main(int argc, char *argv[]) {
    QCachingLocale cl;

    // Merely instantiating QCachingLocale activates it. Use Q_UNUSED to prevent
    // compiler warnings.
    Q_UNUSED(cl);

    // FIXME: argc and argv are not yet being used.
    Q_UNUSED(argc)
    Q_UNUSED(argv);

    QTextStream cout(stdout);
    QTextStream cerr(stderr);
    QTime timer;

    EpisodesParser::Parser * parser;
    Analytics::Analyst * analyst;

    EpisodesParser::Parser::initParserHelpers("./config/browscap.csv",
                                              "./config/browscap-index.db",
                                              "./config/GeoIPCity.dat",
                                              "./config/GeoIPASNum.dat",
                                              "./config/EpisodesSpeeds.csv"
                                              );

    // Instantiate the EpisodesParser and the Analytics. Then connect them.
    parser = new EpisodesParser::Parser();
    // TODO: when these parameters have been figured out, they should be the defaults
    // and therefor they should be moved to the Analyst constructor.
    analyst = new Analytics::Analyst(0.05, 0.04, 0.2);

    // Set constraints. This defines which associations will be found. By
    // default, only causes for slow episodes will be searched.
    analyst->addFrequentItemsetItemConstraint("episode:*", Analytics::CONSTRAINT_POSITIVE_MATCH_ANY);
    analyst->addRuleConsequentItemConstraint("duration:slow", Analytics::CONSTRAINT_POSITIVE_MATCH_ANY);
    //analyst->addRuleConsequentItemConstraint("duration:acceptable", Analytics::CONSTRAINT_POSITIVE_MATCH_ANY);
    //analyst->addRuleConsequentItemConstraint("duration:fast", Analytics::CONSTRAINT_POSITIVE_MATCH_ANY);

    QObject::connect(parser, SIGNAL(processedChunk(QList<QStringList>, double)), analyst, SLOT(analyzeTransactions(QList<QStringList>, double)));

    timer.start();

    // Execute the EpisodesParser. This will generate transactions. These will
    // automatically be sent to Analytics' Analyst.
    int linesParsed = parser->parse("/Users/wimleers/School/masterthesis/logs/driverpacks.net.episodes.log");
    int timePassed = timer.elapsed();
    cout << QString("Duration: %1 ms. Parsed %2 lines. That's %3 lines/ms or %4 ms/line.")
            .arg(timePassed)
            .arg(linesParsed)
            .arg((float)linesParsed/timePassed)
            .arg((float)timePassed/linesParsed)
            << endl;
    delete parser;

    EpisodesParser::Parser::clearParserHelperCaches();

    // Mine association rules.
    analyst->mineRules(0, (uint) TTW_NUM_BUCKETS - 1); // Rules over the entire dataset.
    analyst->mineRules(4, 4); // Rules over the past hour.

    // FIXME: remove this eventually. This is merely here to check on memory
    // consumption after everything has been deleted from memory.
    QTextStream in(stdin);
    forever {
        QString line = in.readLine();
        if (!line.isNull())
            break;
    }
}



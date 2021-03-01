//
// Created by Kolby on 6/19/2019.
//

#include <QWidget>
#include <QDialog>

#include "startoptions.h"
#include <startoptionsrevealed.h>
#include <startoptionssort.h>
#include <startoptionsrestore.h>


#include "wallet/bip39.h"


class WalletModel;

namespace Ui {
    class StartOptionsMain;
}

/** Dialog to ask for passphrases. Used for encryption only
 */
class StartOptionsMain : public QDialog {
    Q_OBJECT

public:
    explicit StartOptionsMain(QWidget *parent);
    ~StartOptionsMain();

    std::vector<std::string> getWords() { return wordsDone; }


public Q_SLOTS:
    void on_RestoreWallet_clicked();
    void on_Next_clicked();
    void on_Back_clicked();
    void on_NewWallet_clicked();

private:
    Ui::StartOptionsMain *ui;
    QWidget* parent;
    int pageNum = 1;
    int rows;
    QStringList qWordList;
    std::vector<std::string> wordsList;
    std::vector<std::string> words;
    std::vector<std::string> wordsDone;

    StartOptions *startOptions;
    StartOptionsRevealed *startOptionsRevealed;
    StartOptionsSort *startOptionsSort;
    StartOptionsRestore *startOptionsRestore;
    std::string words_empty_str;
    std::string words_mnemonic;


};



//
// Created by Kolby on 6/19/2019.
//

#include <QLabel>
#include <QWidget>
#include "wallet/bip39.h"

namespace Ui {
    class StartOptionsRevealed;
}

/** Dialog to ask for passphrases. Used for encryption only
 */
class StartOptionsRevealed : public QWidget {
    Q_OBJECT

public:
    explicit StartOptionsRevealed(std::vector<std::string>& Words, int rows, QWidget *parent = nullptr);
    ~StartOptionsRevealed();


private:
    Ui::StartOptionsRevealed *ui;
    std::list<QLabel*> labelsList;

};

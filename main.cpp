#include <QApplication>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QDebug>
#include <QRegularExpression>


class CalculatorModel : public QObject {
    Q_OBJECT

public:
    CalculatorModel(QObject *parent = nullptr) : QObject(parent) {
        reset();
    }

    void addInput(const QString &input) {
        if (input == "C") {
            reset();
        } else if (input == "=") {
            calculateResult();
        } else {
            if (currentExpression == "0") {
                currentExpression = input;
            } else {
                currentExpression += input;
            }
        }
        emit displayChanged(currentExpression);
    }

    QString getDisplayText() const {
        return currentExpression;
    }

signals:
    void displayChanged(const QString &newDisplay);

private:
    QString currentExpression;

    void reset() {
        currentExpression = "0";
        emit displayChanged(currentExpression);
    }

    void calculateResult() {
        QRegularExpression regex(R"((\d+)([\+\-\*\/])(\d+))");
        QRegularExpressionMatch match = regex.match(currentExpression);

        if (match.hasMatch()) {
            double leftOperand = match.captured(1).toDouble();
            QString operatorStr = match.captured(2);
            double rightOperand = match.captured(3).toDouble();
            double result = 0.0;

            if (operatorStr == "+") {
                result = leftOperand + rightOperand;
            } else if (operatorStr == "-") {
                result = leftOperand - rightOperand;
            } else if (operatorStr == "*") {
                result = leftOperand * rightOperand;
            } else if (operatorStr == "/") {
                if (rightOperand != 0.0) {
                    result = leftOperand / rightOperand;
                } else {
                    currentExpression = "Error: Div/0";
                    emit displayChanged(currentExpression);
                    return;
                }
            }
            currentExpression = QString::number(result);
        } else {
            currentExpression = "Error";
        }
        emit displayChanged(currentExpression);
    }
};

class Calculator : public QWidget {
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);

private slots:
    void handleButtonClicked();

private:
    QLineEdit *display;
    CalculatorModel *model;
};

Calculator::Calculator(QWidget *parent) : QWidget(parent) {
    model = new CalculatorModel(this);

    QGridLayout *mainLayout = new QGridLayout(this);

    display = new QLineEdit(this);
    display->setAlignment(Qt::AlignRight);
    display->setReadOnly(true);
    mainLayout->addWidget(display, 0, 0, 1, 4);

    QStringList buttonLabels = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "+", "-", "*", "/", "C", "="
    };

    int positions[16][2] = {
        {4, 1}, {3, 0}, {3, 1}, {3, 2}, {2, 0}, {2, 1}, {2, 2},
        {1, 0}, {1, 1}, {1, 2}, {4, 3}, {3, 3}, {2, 3}, {1, 3},
        {4, 0}, {4, 2}
    };

    for (int i = 0; i < buttonLabels.size(); ++i) {
        QPushButton *button = new QPushButton(buttonLabels[i], this);
        mainLayout->addWidget(button, positions[i][0], positions[i][1]);

        connect(button, &QPushButton::clicked, this, &Calculator::handleButtonClicked);
    }

    connect(model, &CalculatorModel::displayChanged, display, &QLineEdit::setText);

    setLayout(mainLayout);

    display->setText(model->getDisplayText());
}

void Calculator::handleButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString buttonText = button->text();
        model->addInput(buttonText);
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Calculator w;
    w.setWindowTitle("Calculator");
    w.show();
    return a.exec();
}

#include "main.moc"

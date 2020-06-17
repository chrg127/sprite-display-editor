#include <QValidator>

class HexValidator : public QValidator {
public:
    HexValidator(QObject *parent)
        : QValidator(parent)
    { }

private:

    static QChar fixupchar(QChar c)
    {
        if (!isxdigit(c.toLatin1()))
            c = ' ';
        if (c.isLower())
            c = c.toUpper();
        return c;
    }

    void fixup(QString &input) const
    {
        for (int i = 0; i < input.size(); i++)
            input[i] = fixupchar(input[i]);
    }

    State validate(QString &input, int &pos) const
    {
        for (int i = 0; i < input.size(); i++) {
            if (!isxdigit(input[i].toLatin1()) && !input[i].isSpace())
                return Invalid;
            input[i] = fixupchar(input[i]);
        }
        return Acceptable;
    }
};


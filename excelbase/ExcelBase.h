#ifndef EXCELREADER_H
#define EXCELREADER_H
#include <QObject>
#include <QAxObject>
#include <QString>
#include <QStringList>
#include <QVariant>

class ExcelBasePrivate;
///
/// \brief win excel
/// https://github.com/qtcn/tianchi/blob/v0.0.2-build20130701/include/tianchi/file/tcmsexcel.h
///
class ExcelBase : public QObject
{
public:
    ExcelBase(QObject* par=NULL);
    ~ExcelBase();
private:
    Q_DISABLE_COPY(ExcelBase)
    Q_DECLARE_PRIVATE(ExcelBase)
    ExcelBasePrivate* const d_ptr;
public:
    /// @brief direction
    enum Alignment
    {
        xlTop    = -4160, ///< up
        xlLeft   = -4131, ///< left
        xlRight  = -4152, ///< right
        xlCenter = -4108, ///< center
        xlBottom = -4107  ///< down
    };

    bool create(const QString& filename = QString());

    bool open(const QString& filename = QString());

    void saveAs(const QString& filename);
    void save();

    void close();

    void kick();

    void setVisible(bool value);
    /// @brief set Excel Title
    void setCaption(const QString& value);

    bool addBook();

    int sheetCount();

    QStringList sheetNames();

    bool currentSheet();

    bool setCurrentSheet(int index);

    QString currentSheetName();

    QVariant read(int row, int col);

    /// @brief
    /// @param [in] row  1
    /// @param [in] col  1
    /// @return
    inline QVariant cell(int row, int col) { return read(row, col); }

    /// @brief
    /// @param [in] row   1 ...
    /// @param [in] col   1 ...
    /// @param [in] value
    void write(int row, int col, const QVariant& value);

    void cellFormat(int row, int col, const QString& format);
    void cellAlign(int row, int col, Alignment hAlign, Alignment vAlign);

    /// @brief
    /// @see rowStart() const
    /// @see rowEnd() const
    /// @see colStart() const
    /// @see colEnd() const
    bool usedRange(int& rowStart, int& colStart, int &rowEnd, int &colEnd);

    QVariant readAll();
    void readAll(QList<QList<QVariant> > &cells);
    bool writeCurrentSheet(const QList<QList<QVariant> > &cells);

    static void convertToColName(int data, QString &res);
    static QString to26AlphabetString(int data);
    static void castListListVariant2Variant(const QList<QList<QVariant> > &cells,QVariant& res);
    static void castVariant2ListListVariant(const QVariant& var,QList<QList<QVariant> > &res);
};

#endif // EXCELREADER_H

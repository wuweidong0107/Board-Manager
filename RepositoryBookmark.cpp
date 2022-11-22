#include "RepositoryBookmark.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>

class TagState {
private:
	std::vector<ushort> arr;
public:
	void push(QString const &tag)
	{
		if (arr.empty()) {
			arr.reserve(100);
		}
		size_t s = arr.size();
		size_t t = tag.size();
		arr.resize(s + t + 1);
		arr[s] = '/';
		std::copy_n(tag.utf16(), t, &arr[s + 1]);
	}
	void push(QStringView const &tag)
	{
		push(tag.toString());
	}
	void pop()
	{
		size_t s = arr.size();
		while (s > 0) {
			s--;
			if (arr[s] == '/') break;
		}
		arr.resize(s);
	}
	bool is(char const *path) const
	{
		size_t s = arr.size();
		for (size_t i = 0; i < s; i++) {
			if (arr[i] != path[i]) return false;
		}
		return path[s] == 0;
	}
	bool operator == (char const *path) const
	{
		return is(path);
	}
	QString str() const
	{
		return arr.empty() ? QString() : QString::fromUtf16((char16_t const *)&arr[0], arr.size());
	}
};

RepositoryBookmark::RepositoryBookmark()
{
    
}

bool RepositoryBookmark::save(const QString &path, const QList<RepositoryData> *items)
{
    QFile file(path);
    if (file.open(QFile::WriteOnly)) {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement("repositories");
        for (RepositoryData const &item : *items) {
            if (item.name.isEmpty() || item.path.isEmpty()) {
                continue;
            }
            writer.writeStartElement("repository");
            writer.writeAttribute("name", item.name);
            writer.writeStartElement("path");
            writer.writeCharacters(item.path);
            writer.writeEndElement();
            writer.writeEndElement();
        }
        writer.writeEndElement();
        writer.writeEndElement();
        return true;
    }
    return false;
}

QList<RepositoryData> RepositoryBookmark::load(const QString &path)
{
    QList<RepositoryData> items;
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        RepositoryData item;
        QString text;
        TagState state;
        QXmlStreamReader reader(&file);
        reader.setNamespaceProcessing(false);
        QXmlStreamAttributes atts;
        while (!reader.atEnd()) {
            QXmlStreamReader::TokenType tt = reader.readNext();
            switch(tt) {
            case QXmlStreamReader::StartElement:
                state.push(reader.name());
                atts = reader.attributes();
                if (state == "/repositories/repository") {
                    item = RepositoryData();
                    item.name = atts.value("name").toString();
                } else if (state == "/repositories/repository/path") {
                    text = QString();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (state == "/repositories/repository/path") {
					item.path = text;
				} else if (state == "/repositories/repository") {
					items.push_back(item);
				}
                state.pop();
                break;
            case QXmlStreamReader::Characters:
                text.append(reader.text());
                break;
            default:
                break;
            }
        }
    }
    return items;
}

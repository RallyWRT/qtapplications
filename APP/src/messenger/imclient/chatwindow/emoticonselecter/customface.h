 /***************************************************************************
 *   Copyright (C) 2006 by yunfan                                          *
 *   yunfan_zg@163.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef EVACUSTOMFACE_H
#define EVACUSTOMFACE_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>

#define QQ_FACE_THUMBNAIL_SIZE    20

class CustomFace {
public:
	CustomFace();
	CustomFace(const CustomFace&rhs);
	CustomFace(const QString &name, const int groupId = 0);
	CustomFace(const QString &name,
				const QString &shortcut,
				const QString &tip,
				const int frames=0,
				const int groupId = 0);

	const QString id() const; // should be the md5 string representation of the file
	const QString &org() const { return OrgName; }  // original name, md5+extension
	const QString fixed()  const; // thumbnail name
	const QString &shortcut()  const { return Shortcut; } // shortcut
	const QString &tip()  const { return Tip; } // tip
	/*const*/ int numFrames()  const { return FramesCount; } // number of frames
	/*const*/ int groupID()  const { return GroupId; } // groupID id of this custom smiley

	void setOrgName(const QString &name) { OrgName = name; }
	void setShortcut(const QString &shortcut) { Shortcut = shortcut; }
	void setTip(const QString &tip) { Tip = tip; }
	void setNumFrames(const int n) { FramesCount = n; }
	void setGroupID(const int id) { GroupId = id; }

	CustomFace &operator=( const CustomFace &rhs);
private:
	QString OrgName, Shortcut, Tip;
	int FramesCount, GroupId;
};

typedef QList<CustomFace> FaceList;

class CustomFaceConfig {
public:
	CustomFaceConfig(const QString &dir);
	~CustomFaceConfig();

	void createConfig();// create an empty xml config file

	bool addFace(const CustomFace &face);
	CustomFace getFace(const int gId, const int fNo);

	bool moveFaceUp(const int gId, const int fNo);
	bool moveFaceDown(const int gId, const int fNo);
	bool moveFaceTo(const int gId, const int fNo, const int gDestId);

	bool removeFace(const CustomFace &face);
	bool removeFace(const int gId, const int fNo);

	bool updateFaceTip(const int gId, const int fNo, const QString &tip);
	bool updateFaceShortcut(const int gId, const int fNo, const QString &shortcut);

	bool addGroup(const QString &name); // groupID name cannot be an empty string
	bool removeGroup(const int groupIndex);
	bool removeGroup(const QString &name);

	bool renameGroup(const QString &oldName, const QString &newName);
	bool moveChildrenTo(const int srcIndex, const int destIndex);

	int groupIndex( const QString &name);
	 int numGroups() const;
	QString groupName(const int groupIndex); // id starts from 0
	QStringList groupNames();
	FaceList groupMembers(const int groupIndex);

	bool loadXML();
	bool saveXML();
        
        bool loadFromFileSystem(const QString &dir);

	QString toString();
private:
	QString m_Dir;
	QDomDocument *m_Doc;
};

#endif // EVACUSTOMFACE_H


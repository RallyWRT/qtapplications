/*
 ****************************************************************************
 * imuser.cpp
 *
 * Created On: 2010-7-3
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2010-7-3
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#include "imuser.h"

#include "./contactsmanager/contactsmanager.h"


namespace HEHUI {


IMUser *IMUser::imUserInstance = 0;

IMUser * IMUser::instance(){
	if(imUserInstance == NULL){
		imUserInstance = new IMUser();
	}

	return imUserInstance;

}

IMUser::IMUser(const QString & userID, QObject *parent)
	:IMUserBase(userID, parent)
{
	stateAfterLoggedin = IM::ONLINESTATE_ONLINE;

}

IMUser::~IMUser() {
	// TODO Auto-generated destructor stub
}



bool IMUser::loadMyInfoFromLocalDatabase(){

    return ContactsManager::instance()->getMyInfoFormLocalDatabase();
    
}

bool IMUser::saveMyInfoToLocalDatabase(){
    return ContactsManager::instance()->saveMyInfoToDatabase();
}

bool IMUser::setContactSummaryInfo(const QString &contactID, const QString &contactSummaryInfo){
    return ContactsManager::instance()->setContactSummaryInfo(contactID, contactSummaryInfo);
}

bool IMUser::saveContactInfoToLocalDatabase(const QString &contactID){
    return ContactsManager::instance()->saveContactInfoToDatabase(contactID);
    
}

IM::OnlineState IMUser::getStateAfterLoggedin() const{
    return stateAfterLoggedin;
}

void IMUser::setStateAfterLoggedin(IM::OnlineState state){
    this->stateAfterLoggedin = state;
}

QStringList IMUser::contactGroups() const{
    QStringList groups =  ContactsManager::instance()->contactGroups();
    if(groups.isEmpty()){
        groups.append(getDefaultGroupName());
    }

    return groups;
}

//bool IMUser::addGroup(const QString &groupName, const QStringList &members){
//    QHash<QString, QStringList> personalContactGroups = getPersonalContactGroupsHash();
//    personalContactGroups.insert(groupName, members);
//    setPersonalContactGroupsHash(personalContactGroups);

//    updatePersonalContactGroupsInfoVersion();

//    return true;
//}

//bool IMUser::removeGroup(const QString &groupName){
//    QHash<QString, QStringList> personalContactGroups = getPersonalContactGroupsHash();
//    personalContactGroups.remove(groupName);
//    setPersonalContactGroupsHash(personalContactGroups);

//    updatePersonalContactGroupsInfoVersion();

//    return true;
//}

//bool IMUser::updateGroupName(const QString &oldGroupName, const QString &newGroupName){
//    if(newGroupName.contains(GROUP_INFO_SEPARATOR, Qt::CaseInsensitive) || newGroupName.contains(CONTACT_INFO_SEPARATOR, Qt::CaseInsensitive)){
//        return false;
//    }

//    QHash<QString, QStringList> personalContactGroups = getPersonalContactGroupsHash();
//    personalContactGroups.insert(newGroupName, personalContactGroups.value(oldGroupName));
//    personalContactGroups.remove(oldGroupName);
//    setPersonalContactGroupsHash(personalContactGroups);

//    updatePersonalContactGroupsInfoVersion();

//    return true;

//}

//bool IMUser::addContactToGroup(const QString &contactID, const QString &groupName){
//    QHash<QString, QStringList> personalContactGroups = getPersonalContactGroupsHash();
//    QStringList members = personalContactGroups.value(groupName);
//    members.append(contactID);
//    personalContactGroups[groupName] = members;
//    setPersonalContactGroupsHash(personalContactGroups);

//    updatePersonalContactGroupsInfoVersion();

//    return true;

//}

QString IMUser::getContactGroupsInfoString() const{

    return ContactsManager::instance()->getContactGroupsInfoString();

}

QStringList IMUser::getContacts() const{
    return ContactsManager::instance()->getContacts();

}


bool IMUser::hasFriendContact(const QString &contactID){

    return ContactsManager::instance()->hasFriendContact(contactID);
}

QString IMUser::groupNameThatContactBelongsTo(const QString &contactID) const{

    return ContactsManager::instance()->groupNameThatContactBelongsTo(contactID);

}


QString IMUser::databaseColumnName(IM::PropertyIDOfUser propertyID) const{
    
    
    QString columnName = "";
    switch(propertyID){
    case IM::PI_UserID:
        columnName = "UserID";
        break;
    case IM::PI_Password:
        columnName = "UserPassword";
        break;
    case IM::PI_TrueName:
        columnName = "TrueName";
        break;
    case IM::PI_NickName:
        columnName = "NickName";
        break;
    case IM::PI_Gender:
        columnName = "Gender";
        break;
    case IM::PI_Age:
        columnName = "Age";
        break;
    case IM::PI_Face:
        columnName = "Face";
        break;
          
    case IM::PI_InterestGroupsInfoVersion :
        columnName = "InterestGroupsInfoVersion";
        break;  
    case IM::PI_PersonalContactGroupsInfoVersion :
        columnName = "PersonalContactGroupsInfoVersion";
        break;       
    case IM::PI_BlacklistInfoVersion:
        columnName = "BlacklistInfoVersion";
        break;
    case IM::PI_Blacklist:
        columnName = "Blacklist";
        break;
    case IM::PI_PersonalInfoVersion :
        columnName = "PersonalInfoVersion";
        break;  
               

    case IM::PI_HomeAddress:
        columnName = "HomeAddress";
        break;
    case IM::PI_HomePhoneNumber:
        columnName = "HomePhoneNumber";
        break;
    case IM::PI_HomeZipCode:
        columnName = "HomeZipCode";
        break;
    case IM::PI_PersonalHomepage:
        columnName = "PersonalHomepage";
        break;
    case IM::PI_PersonalEmailAddress:
        columnName = "PersonalEmailAddress";
        break;
    case IM::PI_LastLoginTime:
        columnName = "LastLoginTime";
        break;
    case IM::PI_LastLoginHostAddress:
        columnName = "LastLoginHostAddress";
        break;
    case IM::PI_LastLoginHostPort:
        columnName = "LastLoginHostPort";
        break;
    case IM::PI_QuestionForSecurity:
        columnName = "QuestionForSecurity";
        break;
    case IM::PI_AnswerForSecurity:
        columnName = "AnswerForSecurity";
        break;
    case IM::PI_EmailForSecurity:
        columnName = "EmailForSecurity";
        break;  
        
    case IM::PI_CompanyName:
        columnName = "CompanyName";
        break;
    case IM::PI_JobTitle:
        columnName = "JobTitle";
        break;
    case IM::PI_BusinessAddress:
        columnName = "BusinessAddress";
        break;
    case IM::PI_BusinessPhoneNumber:
        columnName = "BusinessPhoneNumber";
        break;
    case IM::PI_BusinessZipCode:
        columnName = "BusinessZipCode";
        break;
    case IM::PI_BusinessFaxNumber:
        columnName = "BusinessFaxNumber";
        break;
    case IM::PI_BusinessHomepage:
        columnName = "BusinessHomepage";
        break;
    case IM::PI_BusinessEmailAddress:
        columnName = "BusinessEmailAddress";
        break;
    case IM::PI_RegistrationTime:
        columnName = "RegistrationTime";
        break;
    case IM::PI_LoginTimes:
        columnName = "LoginTimes";
        break;
    case IM::PI_FriendshipApply:
        columnName = "FriendshipApply";
        break;    
    case IM::PI_ShortTalk:
        columnName = "ShortTalk";
        break;  

    case IM::PI_Role:
        columnName = "Role";
        break;         
        
    default:
        columnName = "";




    }

    return columnName;



}









}

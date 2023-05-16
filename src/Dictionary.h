#ifndef DICTIONARY_H
#define DICTIONARY_H

// MQTT
extern const char StatsTopic[];
extern const char ButtonLeftTopic[];
extern const char ButtonSelectTopic[];
extern const char ButtonRightTopic[];

// Generic
extern const char State0[];
extern const char State1[];

// HA
extern const char HAmanufacturer[];
extern const char HAmodel[];
extern const char HAmatID[];
extern const char HAmatIcon[];
extern const char HAmatName[];

extern const char HAi1ID[];
extern const char HAi1Icon[];
extern const char HAi1Name[];

extern const char HAi2ID[];
extern const char HAi2Icon[];
extern const char HAi2Name[];

extern const char HAi3ID[];
extern const char HAi3Icon[];
extern const char HAi3Name[];

extern const char HAbriID[];
extern const char HAbriIcon[];
extern const char HAbriName[];
extern const char HAbriOptions[];

extern const char HAbtnaID[];
extern const char HAbtnaIcon[];
extern const char HAbtnaName[];

extern const char HAbtnbID[];
extern const char HAbtnbIcon[];
extern const char HAbtnbName[];

extern const char HAbtncID[];
extern const char HAbtncIcon[];
extern const char HAbtncName[];

extern const char HAappID[];
extern const char HAappIcon[];
extern const char HAappName[];

extern const char HAtempID[];
extern const char HAtempIcon[];
extern const char HAtempName[];
extern const char HAtempClass[];
extern const char HAtempUnit[];

extern const char HAhumID[];
extern const char HAhumIcon[];
extern const char HAhumName[];
extern const char HAhumClass[];
extern const char HAhumUnit[];

#ifdef ULANZI
extern const char HAbatID[];
extern const char HAbatIcon[];
extern const char HAbatName[];
extern const char HAbatClass[];
extern const char HAbatUnit[];
#endif

extern const char HAluxID[];
extern const char HAluxIcon[];
extern const char HAluxName[];
extern const char HAluxClass[];
extern const char HAluxUnit[];

extern const char HAverID[];
extern const char HAverName[];

extern const char HAsigID[];
extern const char HAsigIcon[];
extern const char HAsigName[];
extern const char HAsigClass[];
extern const char HAsigUnit[];

extern const char HAupID[];
extern const char HAupName[];
extern const char HAupClass[];

extern const char HAdoUpID[];
extern const char HAdoUpName[];
extern const char HAdoUpIcon[];

extern const char HAtransID[];
extern const char HAtransName[];
extern const char HAtransIcon[];

extern const char HAupdateID[];
extern const char HAupdateName[];
extern const char HAupdateClass[];
extern const char HAupdateIcon[];

extern const char HAbtnLID[];
extern const char HAbtnLName[];

extern const char HAbtnMID[];
extern const char HAbtnMName[];

extern const char HAbtnRID[];
extern const char HAbtnRName[];

extern const char HAramRID[];
extern const char HAramIcon[];
extern const char HAramName[];
extern const char HAramClass[];
extern const char HAramUnit[];

// JSON properites
#ifdef ULANZI
extern const char BatKey[];
extern const char BatRawKey[];
#endif
extern const char LuxKey[];
extern const char LDRRawKey[];
extern const char BrightnessKey[];
extern const char TempKey[];
extern const char HumKey[];
extern const char UpTimeKey[];
extern const char SignalStrengthKey[];
extern const char UpdateKey[];
extern const char MessagesKey[];
extern const char VersionKey[];
extern const char RamKey[];
#endif

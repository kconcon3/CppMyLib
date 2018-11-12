#pragma once

#include "AssertMacro.hpp"

// 前方宣言 
class GeneralBaseClass;

// クラス概要：汎用コンポーネント用の基本クラス 
class GeneralBaseComponent : public Noncopyable
{
///// コンストラクタ・デストラクタ 
protected:
	GeneralBaseComponent();
public:
	virtual ~GeneralBaseComponent();
///// 生成・登録・初期化・毎フレーム処理の定義 
public:
	void CheckerComponent() {}
	virtual void Init() = 0;
	virtual void Update() = 0;

///// 機能 
public:
	void SetParentPtr(GeneralBaseClass* pParent);
	GeneralBaseClass* GetParentPtr();

private:
	GeneralBaseClass*	m_pParent;

};


// EOF

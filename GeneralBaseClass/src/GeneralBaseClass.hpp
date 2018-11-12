#pragma once

#include "AssertMacro.hpp"

#include "GeneralBaseComponent.hpp"


// クラス概要：汎用基本クラス 
class GeneralBaseClass : public Noncopyable
{
///// コンストラクタ・デストラクタ 
protected:
	GeneralBaseClass();
public:
	virtual ~GeneralBaseClass();
///// 生成・登録・初期化・毎フレーム処理の定義 
public:
	virtual void Update();

///// 機能 
public:
	// コンポーネント情報用 
	struct ComponentExtInfo
	{
		GeneralBaseComponent*		pComponent;		// コンポーネントのインスタンスのポインタ 
		const std::type_info*		pTypeInfo;		// コンポーネントの型(クラス)のポインタ(*pTypeInfo で型が取れます)

		ComponentExtInfo() :
			pComponent(null_ptr),
			pTypeInfo(null_ptr)
		{}
	};

	const std::vector<ComponentExtInfo> GetComponentInfoList();
	void SetLocalComponentNumberMax(uint64_t max);

///// テンプレートを利用した機能 
private:
	static const uint64_t COMPONENT_MAX = (5);	// コンポーネントの最大数(数は適当です) 

	// コンポーネント情報用 
	struct ComponentInfo
	{
		std::shared_ptr<GeneralBaseComponent>	pComponent;		// コンポーネントのインスタンスのポインタ(本当はunique_ptrにしたいけど) 
		const std::type_info*					pTypeInfo;		// コンポーネントの型(クラス)のポインタ 

		ComponentInfo() :
			pComponent(),
			pTypeInfo(null_ptr)
		{}
	};

	std::vector<ComponentInfo>	m_componentList;
	uint64_t					m_localComponentNumMax;

public:
	/**
	* コンポーネントのインスタンスを生成 
	*
	* @return	T_*		生成したコンポーネントのインスタンスのポインタ 
	*/
	template<class T_>
	T_* CreateComponent()
	{
		// CreateComponentの引数無しの場合は最後にインサートさせる為、最大値をセット 
		return CreateComponent<T_>(COMPONENT_MAX);
	}

	/**
	* コンポーネントのインスタンスを生成
	*
	* @param[in]	uint64_t insertIdx	生成したコンポーネントの処理の順番(Updateの順番) 
	*
	* @return	T_*		生成したコンポーネントのインスタンスのポインタ
	*/
	template<class T_>
	T_* CreateComponent(uint64_t insertIdx)
	{
		ASSERT_PRINT(m_componentList.size()<COMPONENT_MAX, "size = %zu", m_componentList.size());
		ASSERT_PRINT(m_componentList.size()<m_localComponentNumMax, "size = %zu / m_localComponentNumMax = %llu", m_componentList.size(), m_localComponentNumMax);

		ComponentInfo tmpInfo;
		tmpInfo.pComponent = std::make_shared<T_>();	// インスタンス作成 
		tmpInfo.pComponent->CheckerComponent();			// 作成時チェック 
		tmpInfo.pComponent->SetParentPtr(this);			// コンポーネントに親のポインタ(自身のポインタ)を登録 
		tmpInfo.pComponent->Init();						// 初期化 
		tmpInfo.pTypeInfo = &typeid(T_);				// 渡されたクラスの型情報を保存

		// vectorに差し込む位置 
		int64_t idx = insertIdx;
		if (idx > static_cast<INT64>(m_componentList.size())) idx = m_componentList.size() - 1;
		if( idx < 0 ) idx = 0;

		// vectorに差し込む 
		m_componentList.emplace(m_componentList.begin()+idx, tmpInfo);

		return reinterpret_cast<T_*>(tmpInfo.pComponent.get());
	}

	/**
	* コンポーネントのインスタンスを削除(クラスの型チェック有り) 
	*
	* @param[in]	T_* pComponent	削除したいコンポーネントのインスタンスのポインタ 
	*
	* @return	無し 
	*/
	template<class T_>
	void DeleteComponent(T_* pComponent)
	{
		DeleteComponent<T_>(pComponent, true);
	}

	/**
	* コンポーネントのインスタンスを削除(クラスの型チェック無し)(注：なるべく型チェックがあるDeleteComponentを使用して下さい) 
	*
	* @param[in]	GeneralBaseComponent* pComponent	削除したいコンポーネントのインスタンスのポインタ
	*
	* @return	無し
	*/
	void DeleteComponentWithNoneCheckType(GeneralBaseComponent* pComponent)
	{
		DeleteComponent<GeneralBaseComponent>(pComponent, false);
	}

	/**
	* T_ と同じ型のクラスを検索して、同じ型のコンポーネントのインスタンスを全て削除
	*
	* @return	無し
	*/
	template<class T_>
	void DeleteComponent()
	{
		bool delFlg = false;

		for (std::vector<ComponentInfo>::iterator it = m_componentList.begin(); it != m_componentList.end();)
		{
			if ((*it->pTypeInfo) == typeid(T_))
			{
				it->pComponent.reset();				// 念の為 
				it = m_componentList.erase(it);
				delFlg = true;
			}
			else
			{
				++it;
			}
		}

		if( !delFlg )
		{
			// T_と同じ型のインスタンスが無かった場合はワーニング表示 
			printf("Warning : not DeleteComponent!\n");
		}
	}

	/**
	* 全コンポーネントのインスタンスを削除
	*
	* @return	無し
	*/
	void DeleteAllComponent()
	{
		for(uint64_t i = 0; i < m_componentList.size(); i++)
		{
			// pComponentの中身はスマートポインタなので自動的に解放されるはずだが、念の為 
			m_componentList.at(i).pComponent.reset();
		}

		// クリア 
		m_componentList.clear();
	}

	/**
	* m_componentListのidx番目に登録されているコンポーネントのインスタンスのポインタを返します 
	*
	* @param[in]	uint64_t idx	m_componentListの何番目の情報を取得するか 
	*
	* @return	GeneralBaseComponent*	コンポーネントのインスタンスのポインタ 
	*/
	GeneralBaseComponent* GetComponentPtr(uint64_t idx)
	{
		ASSERT_PRINT(idx<m_componentList.size(), "size = %zu", m_componentList.size());

		return m_componentList.at(idx).pComponent.get();
	}

	/**
	* T_ と同じ型のコンポーネントを検索し、コンポーネントのインスタンスのポインタを返します 
	*  注：この関数(GetComponentPtrList)を呼び出す毎にvector型のリストを作って、なおかつvector型の変数ごと返しているので 
	*      連続して呼び出すと処理が重くなる可能性があります 
	*
	* @return	std::vector<T_*>	コンポーネントのインスタンスのポインタ(複数ある場合があるのでvectorで返してます) 
	*/
	template<class T_>
	std::vector<T_*> GetComponentPtrList()
	{
		std::vector<T_*> tmpList;

		for (std::vector<ComponentInfo>::iterator it = m_componentList.begin(); it != m_componentList.end(); ++it)
		{
			if ((*it->pTypeInfo) == typeid(T_))
			{
				tmpList.emplace_back(reinterpret_cast<T_*>(it->pComponent.get()));
			}
		}

		return tmpList;
	}
private:
	/**
	* コンポーネントのインスタンスを削除 
	*
	* @param[in]	T_* pComponent	削除したいコンポーネントのインスタンスのポインタ
	*
	* @return	無し
	*/
	template<class T_>
	void DeleteComponent(T_* pComponent, bool typeChkFlg)
	{
		if (pComponent == null_ptr) return;

		bool delFlg = false;

		for (std::vector<ComponentInfo>::iterator it = m_componentList.begin(); it != m_componentList.end();)
		{
			if (pComponent == it->pComponent.get())
			{
				if (typeChkFlg) { ASSERT_PRINT(*it->pTypeInfo == typeid(T_), "error"); }

				it->pComponent.reset();		// 念の為 
				it = m_componentList.erase(it);
				delFlg = true;
			}
			else
			{
				++it;
			}
		}

		if (!delFlg)
		{
			// pComponentで指定したインスタンスが無かった場合はワーニング表示 
			printf("Warning : not DeleteComponent!\n");
		}
	}


};


// EOF

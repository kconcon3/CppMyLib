
#include "GeneralBaseClass.hpp"


//********************************************************************************************
/**
 * コンストラクタ 
 *
 *   コンストラクタ 
 *
 * @return		なし
 */
//********************************************************************************************
GeneralBaseClass::GeneralBaseClass() :
	m_componentList(),
	m_localComponentNumMax(COMPONENT_MAX)
{
	m_componentList.reserve(COMPONENT_MAX);
}

//********************************************************************************************
/**
 * デストラクタ 
 *
 *   デストラクタ 
 *
 * @return		なし
 */
//********************************************************************************************
GeneralBaseClass::~GeneralBaseClass()
{
	// 念の為 
	for(ComponentInfo& rInfo : m_componentList)
	{
		rInfo.pComponent.reset();
	}
}

//********************************************************************************************
/**
* 1フレーム毎に呼ばれる処理
*
*   1フレーム毎に呼ばれる処理 
*
* @return		なし
*/
//********************************************************************************************
void GeneralBaseClass::Update()
{
	// m_componentListの順番でUpdateは実行されます 
	for(ComponentInfo& rInfo : m_componentList)
	{
		rInfo.pComponent->Update();
	}
}

//********************************************************************************************
/**
* コンポーネントの情報のリストを取得 
*
*   コンポーネントの情報のリストを取得します 
*   注：この関数を呼ぶ毎にリスト(std::vector)を作成して返しているので、処理が重い場合は複数回呼ばないようにして下さい) 
*
* @return		const std::vector<GeneralBaseClass::ComponentExtInfo>		情報のリスト 
*/
//********************************************************************************************
const std::vector<GeneralBaseClass::ComponentExtInfo> GeneralBaseClass::GetComponentInfoList()
{
	std::vector<ComponentExtInfo> retList;

	for (std::vector<ComponentInfo>::iterator it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		retList.emplace_back();
		ComponentExtInfo& rCont = retList.back();	// emplace_backした中身 

		rCont.pComponent = it->pComponent.get();
		rCont.pTypeInfo = it->pTypeInfo;
	}

	return retList;
}

//********************************************************************************************
/**
* 持てるコンポーネントの最大数をセット 
*
*   持てるコンポーネントの最大数をセット(コンポーネントの数を抑制したい場合などに使用)  
*
* @param[in]	uint64_t	max		コンポーネントの最大数 
*
* @return		無し
*/
//********************************************************************************************
void GeneralBaseClass::SetLocalComponentNumberMax(uint64_t max)
{
	ASSERT_PRINT(max <= COMPONENT_MAX, "max = %llu", max);
	ASSERT_PRINT(max >= m_componentList.size(), "max = %llu | size = %zu", max, m_componentList.size());

	m_localComponentNumMax = max;
}



// EOF

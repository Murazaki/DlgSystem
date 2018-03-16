// Copyright 2017-2018 Csaba Molnar, Daniel Butum
#pragma once

#include "DlgMemory.generated.h"

/**
 *  Data per dialogue asset
 */
USTRUCT(Blueprintable, BlueprintType)
struct FDlgHistory
{
	GENERATED_USTRUCT_BODY()

	/** list of already visited node indices */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DlgHistoryData)
	TSet<int32> VisitedNodeIndices;
};

/**
 *  Singleton to store Dialogue history
 */
class DLGSYSTEM_API DlgMemory
{
public:
	static DlgMemory* GetInstance()
	{
		static DlgMemory Instance;
		return &Instance;
	}

	/** removes all entries */
	void Empty() { HistoryMap.Empty(); }

	/** adds an entry to the map or overrides an existing one */
	void SetEntry(const FGuid& DlgGuid, const FDlgHistory& History);

	/** returns the entry for the given name, or nullptr if it does not exist */
	FDlgHistory* GetEntry(const FGuid& DlgGuid) { return HistoryMap.Find(DlgGuid); }

	void SetNodeVisited(const FGuid& DlgGuid, const int32 NodeIndex);
	bool IsNodeVisited(const FGuid& DlgGuid, const int32 NodeIndex) const;

	const TMap<FGuid, FDlgHistory>& GetHistoryMaps() const { return HistoryMap; }
	void SetHistoryMap(const TMap<FGuid, FDlgHistory>& Map) { HistoryMap = Map; }

	void Serialize(FArchive& Ar);
private:

	/**
	 *  Key: Dialogue unique identifier Guid
	 *  Value: set of already visited nodes
	 */
	UPROPERTY()
	TMap<FGuid, FDlgHistory> HistoryMap;
};

// operator overloads for serialization
FORCEINLINE FArchive& operator<<(FArchive &Ar, FDlgHistory& History)
{
	Ar << History.VisitedNodeIndices;
	return Ar;
}

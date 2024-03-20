#pragma once

#include "aja-props.hpp"
#include "aja-routing.hpp"
#include "aja-routing.hpp"

#include <obs-module.h>

#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2publicinterface.h>

#include <memory>
#include <map>
#include <mutex>
#include <vector>

class CNTV2Card;
class AJAOutput;
class AJASource;

namespace aja {

// using ChannelPwnz = std::map<std::string, int32_t>;
struct ChannelData {
	ChannelData();
	ChannelData(int32_t chans, RoutingPresetID pre);
	ChannelData(const ChannelData &other);
	int32_t channels; // bitfield
	RoutingPresetID presetID;
};

using ChannelPwnz = std::map<std::string, ChannelData>;

/* A CardEntry for each physical AJA card is added to a map retained by the CardManager.
 * Each CardEntry itself maintains a map representing the AJA card "Channels" the are
 * owned by a particular capture or output plugin instance. The Channel ownership map is
 * then used to determine which "IOSelection" (i.e. SDI1, SDI3+4, HDMI Monitor Output, etc.)
 * drop-down menu items are either accessible or grayed out in the capture and output plugin UIs.
 */
class CardEntry {
public:
	CardEntry(uint32_t cardIndex, const std::string &cardID);
	virtual ~CardEntry();
	CNTV2Card *GetCard();
	virtual bool Initialize();
	virtual uint32_t GetCardIndex() const;
	virtual std::string GetCardID() const;
	virtual std::string GetDisplayName() const;
	virtual std::string GetSerial() const;
	virtual NTV2DeviceID GetDeviceID() const;

	// NOTE: Kona5 & io4K+ 6G/12G-SDI Capture and Output
	// Kona5 and io4K+ only support 6G/12G-SDI capture on SDI1 and output on SDI3.
	// Simultaneous 6G/12G-SDI capture and output via SDI1/3 is possible, but if
	// either of these modes are in use SDI2 and SDI4 are effectively locked out.
	virtual bool IsKona5Io4KPlus6g12gCaptureEnabled() const;
	virtual bool IsKona5Io4KPlus6g12gOutputEnabled() const;

	virtual bool FindRoutingPreset(const IOConfig &ioConf,
				       RoutingPreset &rp);
	virtual bool ConfigureRouting(const IOConfig &ioConf,
				      const RoutingPreset &rp, CNTV2Card *card,
				      NTV2XptConnections &cnx);
	virtual bool AcquireChannelsForIOConfig(const IOConfig &ioConf,
						const std::string &owner);
	virtual bool ReleaseChannelsForIOConfig(const IOConfig &ioConf,
						const std::string &owner);

	virtual bool ChannelReady(NTV2Channel chan,
				  const std::string &owner) const;
	virtual bool AcquireChannel(NTV2Channel chan, NTV2Mode mode,
				    const std::string &owner);
	virtual bool ReleaseChannel(NTV2Channel chan, NTV2Mode mode,
				    const std::string &owner);

	virtual void SetActivePreset(RoutingPresetID id,
				     const std::string &owner);
	virtual bool IsPresetActive(RoutingPresetID id) const;

	virtual bool AcquireChannels(const NTV2ChannelList &chans,
				     NTV2Mode mode, const std::string &owner);
	virtual bool ReleaseChannels(const NTV2ChannelList &chans,
				     NTV2Mode mode, const std::string &owner);
	virtual bool ChannelsReady(const NTV2ChannelList &chans,
				   const std::string &owner);

	virtual bool InputSelectionReady(IOSelection io, NTV2DeviceID id,
					 const std::string &owner) const;
	// virtual bool AcquireInputSelection(IOSelection io, NTV2DeviceID id,
	// 				   const std::string &owner);
	// virtual bool ReleaseInputSelection(IOSelection io, NTV2DeviceID id,
	// 				   const std::string &owner);
	// virtual bool AcquireInputSelection(IOSelection io, NTV2DeviceID id,
	// 				   const std::string &owner);
	// virtual bool ReleaseInputSelection(IOSelection io, NTV2DeviceID id,
	// 				   const std::string &owner);
	virtual bool OutputSelectionReady(IOSelection io, NTV2DeviceID id,
					  const std::string &owner) const;
	// virtual bool AcquireOutputSelection(IOSelection io, NTV2DeviceID id,
	// 				    const std::string &owner);
	// virtual bool ReleaseOutputSelection(IOSelection io, NTV2DeviceID id,
	// 				    const std::string &owner);

	// virtual bool AcquireOutputSelection(IOSelection io, NTV2DeviceID id,
	// 				    const std::string &owner);
	// virtual bool ReleaseOutputSelection(IOSelection io, NTV2DeviceID id,
	// 				    const std::string &owner);

	virtual bool UpdateChannelOwnerName(const std::string &oldName,
					    const std::string &newName);

private:
	virtual NTV2ChannelList inputSelectionChannels(IOSelection io,
						       NTV2DeviceID id) const;
	virtual NTV2ChannelList outputSelectionChannels(IOSelection io,
							NTV2DeviceID id) const;
	virtual bool isAutoCirculateRunning(NTV2Channel);

protected:
	uint32_t mCardIndex;
	std::string mCardID;
	std::unique_ptr<CNTV2Card> mCard;
	std::unique_ptr<RoutingManager> mRouting;
	ChannelPwnz mChannelPwnz;
	mutable std::mutex mMutex;
};
using CardEntryPtr = std::shared_ptr<CardEntry>;
using CardEntries = std::map<std::string, CardEntryPtr>;

/* The CardManager enumerates the physical AJA cards in the system, reverts them to a default
 * state on exit, and maintains a map of CardEntry objects corresponding to each physical card.
 * Each CardEntry object holds a pointer to the CNTV2Card instance and a map of NTV2Channels
 * that are "owned" by each plugin instance. NTV2Channels are essentially treated as indices
 * for various firwmare Widgets and sub-systems throughout the AJA NTV2 SDK.
 */
class CardManager {
public:
	static CardManager &Instance();

	void ClearCardEntries();
	void EnumerateCards();

	size_t NumCardEntries() const;
	CNTV2Card *GetCard(const std::string &cardID);
	const CardEntryPtr GetCardEntry(const std::string &cardID) const;
	const CardEntries &GetCardEntries() const;
	const CardEntries::iterator begin();
	const CardEntries::iterator end();

private:
	CardManager() = default;
	~CardManager() = default;
	CardManager(const CardManager &) = delete;
	CardManager(const CardManager &&) = delete;
	CardManager &operator=(const CardManager &) = delete;
	CardManager &operator=(const CardManager &&) = delete;

	CardEntries mCardEntries;
	mutable std::mutex mMutex;
};

} // namespace aja

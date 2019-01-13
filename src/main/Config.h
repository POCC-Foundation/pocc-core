#pragma once
// Copyright 2014 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "crypto/SecretKey.h"
#include "lib/util/cpptoml.h"
#include "overlay/StellarXDR.h"
#include "util/optional.h"
#include <map>
#include <memory>
#include <string>

#define DEFAULT_PEER_PORT 11625

namespace stellar
{
class HistoryArchive;

class Config : public std::enable_shared_from_this<Config>
{
    void validateConfig();
    void loadQset(std::shared_ptr<cpptoml::toml_group> group,
                  SCPQuorumSet& qset, int level);

    void parseNodeID(std::string configStr, PublicKey& retKey);
    void parseNodeID(std::string configStr, PublicKey& retKey, SecretKey& sKey,
                     bool isSeed);

    std::string expandNodeID(std::string const& s) const;

  public:
    typedef std::shared_ptr<Config> pointer;

    enum TestDbMode
    {
        TESTDB_DEFAULT,
        TESTDB_IN_MEMORY_SQLITE,
        TESTDB_ON_DISK_SQLITE,
#ifdef USE_POSTGRES
        TESTDB_POSTGRESQL,
#endif
        TESTDB_MODES
    };

    // application config

    // stellar-core启动的默认方式是从磁盘加载状态并在启动SCP之前赶上网络。 如果您需要不同的行为，则需要使用--newdb或--force-scp设置以下标志：

    // SCP将立即开始使用当前的当地州参与共识。 请勿在配置文件中包含此内容
    bool FORCE_SCP;

    // 这是一种测试模式。 它会阻止您尝试连接到其他对等方
    bool RUN_STANDALONE;

    // 测试模式。 Ledger只会在被告知超过http时关闭
    bool MANUAL_CLOSE;

    // 是否“完全”追赶（重播历史）; default是false，意味着使用增量到最近的快照来“追踪”。
    bool CATCHUP_COMPLETE;

    // 当前分类账之前的“近期”分类账数量包括在“最小”追赶中。 默认值为0，如果CATCHUP_COMPLETE设置为true，则忽略此值。 如果你想要一周的历史记录，请将其设置为120000。
    uint32_t CATCHUP_RECENT;

    // 启用或禁用启动时的自动维护
    bool MAINTENANCE_ON_STARTUP;

    //一个config参数，使用`generateload` runtime命令（请参阅CommandHandler.cpp）按需生成合成负载。 此选项仅用于压力测试，不应在生产网络中启用。
    bool ARTIFICIALLY_GENERATE_LOAD_FOR_TESTING;

    // 一个配置参数，可将分类帐关闭时间减少到1秒，检查点频率减少到每8个分类帐。 不要将其设置为生产中，因为它会使您的历史档案与其他任何人的档案不兼容。
    bool ARTIFICIALLY_ACCELERATE_TIME_FOR_TESTING;

    // 一个config参数，用于覆盖关闭时间（以秒为单位）。 不要在生产中使用，因为它可能会使网络不稳定。
    uint32 ARTIFICIALLY_SET_CLOSE_TIME_FOR_TESTING;

    // 一个config参数，它避免在将FutureBuckets写入数据库的持久状态之前解析它; 此选项仅用于压力测试从中断合并恢复的能力，并且在所有正常情况下都应为false。
    bool ARTIFICIALLY_PESSIMIZE_MERGES_FOR_TESTING;

    // 允许连接到localhost的配置只应在测试时启用，因为这是一个安全问题
    bool ALLOW_LOCALHOST_FOR_TESTING;

    // 这是您希望能够容忍的失败次数。 您的仲裁集中至少需要3f + 1个节点。 如果您的仲裁设置不足以容忍您在此处设置的级别，则stellar-core将无法运行。
    int32_t FAILURE_SAFETY;

    // 如果设置为true，则允许您指定不安全的仲裁集。 否则，如果您将阈值％设置得太低，它将无法启动。 如果您正在运行自己的网络并且不关心拜占庭故障，则可能需要设置此项。
    bool UNSAFE_QUORUM;

    uint32_t LEDGER_PROTOCOL_VERSION;
    optional<std::tm> PREFERRED_UPGRADE_DATETIME;

    // 注意：范围内的所有版本
    // [OVERLAY_PROTOCOL_MIN_VERSION, OVERLAY_PROTOCOL_VERSION] must be handled
    uint32_t OVERLAY_PROTOCOL_MIN_VERSION; // 最小叠加版本了解
    uint32_t OVERLAY_PROTOCOL_VERSION;     // 最大叠加版本了解
    std::string VERSION_STR;
    std::string LOG_FILE_PATH;
    std::string BUCKET_DIR_PATH;
    uint32_t DESIRED_BASE_FEE;     // in stroops
    uint32_t DESIRED_BASE_RESERVE; // in stroops
    uint32_t DESIRED_MAX_TX_PER_LEDGER;
    unsigned short HTTP_PORT; // 什么端口来监听命令
    bool PUBLIC_HTTP_PORT;    // 如果您接受来自localhost的命令
    int HTTP_MAX_CLIENT;      // http客户端的最大数量，即积压
    std::string NETWORK_PASSPHRASE; // 网络的标识符

    // 覆盖配置
    unsigned short PEER_PORT;
    unsigned TARGET_PEER_CONNECTIONS;
    unsigned MAX_PEER_CONNECTIONS;
    // 同行们，我们将始终努力保持联系
    std::vector<std::string> PREFERRED_PEERS;
    std::vector<std::string> KNOWN_PEERS;

    // 偏好也可以由peer pubkey表示
    std::vector<std::string> PREFERRED_PEER_KEYS;

    // 是否排除不喜欢的同行。
    bool PREFERRED_PEERS_ONLY;

    // 下面的系统活动百分比（在事件循环周期和数据库时间方面测量）在0到100之间 - 系统将认为自己“加载”并试图减少负载。 将此数字设置为低，系统将容忍过载。 将其设置为高，系统将不会容忍。 默认情况下它为0，表示对重载完全不敏感。
    uint32_t MINIMUM_IDLE_PERCENT;

    // 进程管理配置
    size_t MAX_CONCURRENT_SUBPROCESSES;

    // 设置此值会导致发生各种额外检查，开销可能导致较慢的系统无法像网络其他部分那样快速执行，建议在使用时谨慎。
    bool PARANOID_MODE;

    // SCP 配置
    SecretKey NODE_SEED;
    bool NODE_IS_VALIDATOR;
    stellar::SCPQuorumSet QUORUM_SET;

    std::map<std::string, std::string> VALIDATOR_NAMES;

    // History 配置
    std::map<std::string, std::shared_ptr<HistoryArchive>> HISTORY;

    // Database 配置
    std::string DATABASE;

    std::vector<std::string> COMMANDS;
    std::vector<std::string> REPORT_METRICS;

    std::string NTP_SERVER; // ntp服务器用于检查主机上的时间是否有效

    Config();

    void load(std::string const& filename);

    std::string toShortString(PublicKey const& pk) const;
    std::string toStrKey(PublicKey const& pk, bool& isAlias) const;
    std::string toStrKey(PublicKey const& pk) const;
    bool resolveNodeID(std::string const& s, PublicKey& retKey) const;
};
}

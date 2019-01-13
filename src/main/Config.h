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

    // stellar-core������Ĭ�Ϸ�ʽ�ǴӴ��̼���״̬��������SCP֮ǰ�������硣 �������Ҫ��ͬ����Ϊ������Ҫʹ��--newdb��--force-scp�������±�־��

    // SCP��������ʼʹ�õ�ǰ�ĵ����ݲ��빲ʶ�� �����������ļ��а���������
    bool FORCE_SCP;

    // ����һ�ֲ���ģʽ�� ������ֹ���������ӵ������Եȷ�
    bool RUN_STANDALONE;

    // ����ģʽ�� Ledgerֻ���ڱ���֪����httpʱ�ر�
    bool MANUAL_CLOSE;

    // �Ƿ���ȫ��׷�ϣ��ز���ʷ��; default��false����ζ��ʹ������������Ŀ�������׷�١���
    bool CATCHUP_COMPLETE;

    // ��ǰ������֮ǰ�ġ����ڡ����������������ڡ���С��׷���С� Ĭ��ֵΪ0�����CATCHUP_COMPLETE����Ϊtrue������Դ�ֵ�� �������Ҫһ�ܵ���ʷ��¼���뽫������Ϊ120000��
    uint32_t CATCHUP_RECENT;

    // ���û��������ʱ���Զ�ά��
    bool MAINTENANCE_ON_STARTUP;

    //һ��config������ʹ��`generateload` runtime��������CommandHandler.cpp���������ɺϳɸ��ء� ��ѡ�������ѹ�����ԣ���Ӧ���������������á�
    bool ARTIFICIALLY_GENERATE_LOAD_FOR_TESTING;

    // һ�����ò������ɽ������ʹر�ʱ����ٵ�1�룬����Ƶ�ʼ��ٵ�ÿ8�������ʡ� ��Ҫ��������Ϊ�����У���Ϊ����ʹ������ʷ�����������κ��˵ĵ��������ݡ�
    bool ARTIFICIALLY_ACCELERATE_TIME_FOR_TESTING;

    // һ��config���������ڸ��ǹر�ʱ�䣨����Ϊ��λ���� ��Ҫ��������ʹ�ã���Ϊ�����ܻ�ʹ���粻�ȶ���
    uint32 ARTIFICIALLY_SET_CLOSE_TIME_FOR_TESTING;

    // һ��config�������������ڽ�FutureBucketsд�����ݿ�ĳ־�״̬֮ǰ������; ��ѡ�������ѹ�����Դ��жϺϲ��ָ���������������������������¶�ӦΪfalse��
    bool ARTIFICIALLY_PESSIMIZE_MERGES_FOR_TESTING;

    // �������ӵ�localhost������ֻӦ�ڲ���ʱ���ã���Ϊ����һ����ȫ����
    bool ALLOW_LOCALHOST_FOR_TESTING;

    // ������ϣ���ܹ����̵�ʧ�ܴ����� �����ٲü���������Ҫ3f + 1���ڵ㡣 ��������ٲ����ò������������ڴ˴����õļ�����stellar-core���޷����С�
    int32_t FAILURE_SAFETY;

    // �������Ϊtrue����������ָ������ȫ���ٲü��� �������������ֵ�����õ�̫�ͣ������޷������� ��������������Լ������粢�Ҳ����İ�ռͥ���ϣ��������Ҫ���ô��
    bool UNSAFE_QUORUM;

    uint32_t LEDGER_PROTOCOL_VERSION;
    optional<std::tm> PREFERRED_UPGRADE_DATETIME;

    // ע�⣺��Χ�ڵ����а汾
    // [OVERLAY_PROTOCOL_MIN_VERSION, OVERLAY_PROTOCOL_VERSION] must be handled
    uint32_t OVERLAY_PROTOCOL_MIN_VERSION; // ��С���Ӱ汾�˽�
    uint32_t OVERLAY_PROTOCOL_VERSION;     // �����Ӱ汾�˽�
    std::string VERSION_STR;
    std::string LOG_FILE_PATH;
    std::string BUCKET_DIR_PATH;
    uint32_t DESIRED_BASE_FEE;     // in stroops
    uint32_t DESIRED_BASE_RESERVE; // in stroops
    uint32_t DESIRED_MAX_TX_PER_LEDGER;
    unsigned short HTTP_PORT; // ʲô�˿�����������
    bool PUBLIC_HTTP_PORT;    // �������������localhost������
    int HTTP_MAX_CLIENT;      // http�ͻ��˵��������������ѹ
    std::string NETWORK_PASSPHRASE; // ����ı�ʶ��

    // ��������
    unsigned short PEER_PORT;
    unsigned TARGET_PEER_CONNECTIONS;
    unsigned MAX_PEER_CONNECTIONS;
    // ͬ���ǣ����ǽ�ʼ��Ŭ��������ϵ
    std::vector<std::string> PREFERRED_PEERS;
    std::vector<std::string> KNOWN_PEERS;

    // ƫ��Ҳ������peer pubkey��ʾ
    std::vector<std::string> PREFERRED_PEER_KEYS;

    // �Ƿ��ų���ϲ����ͬ�С�
    bool PREFERRED_PEERS_ONLY;

    // �����ϵͳ��ٷֱȣ����¼�ѭ�����ں����ݿ�ʱ�䷽���������0��100֮�� - ϵͳ����Ϊ�Լ������ء�����ͼ���ٸ��ء� ������������Ϊ�ͣ�ϵͳ�����̹��ء� ��������Ϊ�ߣ�ϵͳ���������̡� Ĭ���������Ϊ0����ʾ��������ȫ�����С�
    uint32_t MINIMUM_IDLE_PERCENT;

    // ���̹�������
    size_t MAX_CONCURRENT_SUBPROCESSES;

    // ���ô�ֵ�ᵼ�·������ֶ����飬�������ܵ��½�����ϵͳ�޷�����������������������ִ�У�������ʹ��ʱ������
    bool PARANOID_MODE;

    // SCP ����
    SecretKey NODE_SEED;
    bool NODE_IS_VALIDATOR;
    stellar::SCPQuorumSet QUORUM_SET;

    std::map<std::string, std::string> VALIDATOR_NAMES;

    // History ����
    std::map<std::string, std::shared_ptr<HistoryArchive>> HISTORY;

    // Database ����
    std::string DATABASE;

    std::vector<std::string> COMMANDS;
    std::vector<std::string> REPORT_METRICS;

    std::string NTP_SERVER; // ntp���������ڼ�������ϵ�ʱ���Ƿ���Ч

    Config();

    void load(std::string const& filename);

    std::string toShortString(PublicKey const& pk) const;
    std::string toStrKey(PublicKey const& pk, bool& isAlias) const;
    std::string toStrKey(PublicKey const& pk) const;
    bool resolveNodeID(std::string const& s, PublicKey& retKey) const;
};
}

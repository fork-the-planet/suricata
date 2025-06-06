/* Copyright (C) 2020 Open Information Security Foundation
 *
 * You can copy, redistribute or modify this Program under the terms of
 * the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/**
 *
 * \author Frank Honza <frank.honza@dcso.de>
 */

#include "suricata-common.h"
#include "conf.h"
#include "detect.h"
#include "detect-parse.h"
#include "detect-engine.h"
#include "detect-engine-content-inspection.h"
#include "detect-ike-nonce-payload-length.h"
#include "app-layer-parser.h"
#include "util-byte.h"
#include "detect-engine-uint.h"

#include "rust-bindings.h"

/**
 *   [ike.nonce_payload_length]:[=|<|>|<=|>=]<length>;
 */
static int DetectIkeNoncePayloadLengthSetup(DetectEngineCtx *, Signature *s, const char *str);
static void DetectIkeNoncePayloadLengthFree(DetectEngineCtx *, void *);
static int g_ike_nonce_payload_length_buffer_id = 0;

static int DetectIkeNoncePayloadLengthMatch(DetectEngineThreadCtx *, Flow *, uint8_t, void *,
        void *, const Signature *, const SigMatchCtx *);

/**
 * \brief Registration function for ike.nonce_payload_length keyword.
 */
void DetectIkeNoncePayloadLengthRegister(void)
{
    sigmatch_table[DETECT_IKE_NONCE_PAYLOAD_LENGTH].name = "ike.nonce_payload_length";
    sigmatch_table[DETECT_IKE_NONCE_PAYLOAD_LENGTH].desc = "match IKE nonce payload length";
    sigmatch_table[DETECT_IKE_NONCE_PAYLOAD_LENGTH].url =
            "/rules/ike-keywords.html#ike-nonce-payload-length";
    sigmatch_table[DETECT_IKE_NONCE_PAYLOAD_LENGTH].AppLayerTxMatch =
            DetectIkeNoncePayloadLengthMatch;
    sigmatch_table[DETECT_IKE_NONCE_PAYLOAD_LENGTH].Setup = DetectIkeNoncePayloadLengthSetup;
    sigmatch_table[DETECT_IKE_NONCE_PAYLOAD_LENGTH].Free = DetectIkeNoncePayloadLengthFree;

    DetectAppLayerInspectEngineRegister("ike.nonce_payload_length", ALPROTO_IKE, SIG_FLAG_TOSERVER,
            1, DetectEngineInspectGenericList, NULL);

    DetectAppLayerInspectEngineRegister("ike.nonce_payload_length", ALPROTO_IKE, SIG_FLAG_TOCLIENT,
            1, DetectEngineInspectGenericList, NULL);

    g_ike_nonce_payload_length_buffer_id = DetectBufferTypeGetByName("ike.nonce_payload_length");
}

/**
 * \internal
 * \brief Function to match nonce length of a IKE state
 *
 * \param det_ctx Pointer to the pattern matcher thread.
 * \param f       Pointer to the current flow.
 * \param flags   Flags.
 * \param state   App layer state.
 * \param txv     Pointer to the Ike Transaction.
 * \param s       Pointer to the Signature.
 * \param ctx     Pointer to the sigmatch that we will cast into DetectU32Data.
 *
 * \retval 0 no match.
 * \retval 1 match.
 */
static int DetectIkeNoncePayloadLengthMatch(DetectEngineThreadCtx *det_ctx, Flow *f, uint8_t flags,
        void *state, void *txv, const Signature *s, const SigMatchCtx *ctx)
{
    SCEnter();

    uint32_t length;
    if (!SCIkeStateGetNoncePayloadLength(txv, &length))
        SCReturnInt(0);
    const DetectU32Data *du32 = (const DetectU32Data *)ctx;
    return DetectU32Match(length, du32);
}

/**
 * \brief Function to add the parsed IKE nonce length field into the current signature.
 *
 * \param de_ctx Pointer to the Detection Engine Context.
 * \param s      Pointer to the Current Signature.
 * \param rawstr Pointer to the user provided flags options.
 *
 * \retval 0 on Success.
 * \retval -1 on Failure.
 */
static int DetectIkeNoncePayloadLengthSetup(
        DetectEngineCtx *de_ctx, Signature *s, const char *rawstr)
{
    if (SCDetectSignatureSetAppProto(s, ALPROTO_IKE) != 0)
        return -1;

    DetectU32Data *nonce_payload_length = DetectU32Parse(rawstr);
    if (nonce_payload_length == NULL)
        return -1;

    /* okay so far so good, lets get this into a SigMatch
     * and put it in the Signature. */

    if (SCSigMatchAppendSMToList(de_ctx, s, DETECT_IKE_NONCE_PAYLOAD_LENGTH,
                (SigMatchCtx *)nonce_payload_length,
                g_ike_nonce_payload_length_buffer_id) == NULL) {
        goto error;
    }
    return 0;

error:
    DetectIkeNoncePayloadLengthFree(de_ctx, nonce_payload_length);
    return -1;
}

/**
 * \internal
 * \brief Function to free memory associated with DetectU32Data.
 *
 * \param de_ptr Pointer to DetectU32Data.
 */
static void DetectIkeNoncePayloadLengthFree(DetectEngineCtx *de_ctx, void *ptr)
{
    SCDetectU32Free(ptr);
}

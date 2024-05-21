/*
 * copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * FFmpeg是自由软件；您可以根据自由软件基金会发布的GNU Lesser General Public License（GNU宽通用公共许可证）
 * 的条款重新分发和/或修改它；许可证的2.1版或（根据您的选择）任何更高版本。
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * FFmpeg的分发是希望它有用，但没有任何保证；甚至没有对适销性或特定用途适用性的暗示保证。有关更多详细信息，
 * 请参阅GNU Lesser General Public License。
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 您应该已经收到 GNU 较宽通用公共许可证和 FFmpeg 的副本； 如果没有，请写信给自由软件基金会，公司，
 * 51富兰克林街，五楼，波士顿，MA02110-1301USA
 */

#ifndef AVFORMAT_AVFORMAT_H
#define AVFORMAT_AVFORMAT_H

/**
 * @file
 * @ingroup libavf
 * Main libavformat public API header
 */

/**
 * @defgroup libavf libavformat
 * I/O and Muxing/Demuxing Library
 *
 * Libavformat (lavf) is a library for dealing with various media container
 * formats. Its main two purposes are demuxing - i.e. splitting a media file
 * into component streams, and the reverse process of muxing - writing supplied
 * data in a specified container format. It also has an @ref lavf_io
 * "I/O module" which supports a number of protocols for accessing the data (e.g.
 * file, tcp, http and others).
 * Unless you are absolutely sure you won't use libavformat's network
 * capabilities, you should also call avformat_network_init().
 * Libavform（lavf）是一个用于处理各种媒体容器格式的库。它的两个主要目的是解复用——即将媒体文件拆分为组件流，
 * 以及复用——以指定的容器格式写入提供的数据。它还有一个@ref lavf_io“I/O模块”，支持许多访问数据的协议（例如文件、tcp、http等）。
 * 除非你绝对确定你不会使用libavform的网络功能，否则你也应该调用avformat_network_init（）。
 *
 * A supported input format is described by an AVInputFormat struct, conversely
 * an output format is described by AVOutputFormat. You can iterate over all
 * input/output formats using the  av_demuxer_iterate / av_muxer_iterate() functions.
 * The protocols layer is not part of the public API, so you can only get the names
 * of supported protocols with the avio_enum_protocols() function.
 * 支持的输入格式由AVInputFormat结构描述，相反，输出格式由AVOutputFormat描述。您可以使用av_demuxer_iterate/av_muxer_iterate（）
 * 函数遍历所有输入/输出格式。协议层不是公共API的一部分，因此您只能使用avio_enum_protocols（）函数获取支持的协议的名称。
 *
 * Main lavf structure used for both muxing and demuxing is AVFormatContext,
 * which exports all information about the file being read or written. As with
 * most Libavformat structures, its size is not part of public ABI, so it cannot be
 * allocated on stack or directly with av_malloc(). To create an
 * AVFormatContext, use avformat_alloc_context() (some functions, like
 * avformat_open_input() might do that for you).
 * 用于复用和解复用的主要lavf结构是AVFormatContext，它导出有关正在读取或写入的文件的所有信息。
 * 就像大多数LibavForforms结构一样，它的大小不是公共ABI的一部分，因此它不能在堆栈上或直接用av_malloc（）分配。
 * 要创建AVFormatContext，请使用avformat_alloc_context（）（某些函数，如avformat_open_input（）可能会为您执行此操作）。
 *
 * Most importantly an AVFormatContext contains:
 * @li the @ref AVFormatContext.iformat "input" or @ref AVFormatContext.oformat
 * "output" format. It is either autodetected or set by user for input;
 * always set by user for output.
 * @li an @ref AVFormatContext.streams "array" of AVStreams, which describe all
 * elementary streams stored in the file. AVStreams are typically referred to
 * using their index in this array.
 * @li an @ref AVFormatContext.pb "I/O context". It is either opened by lavf or
 * set by user for input, always set by user for output (unless you are dealing
 * with an AVFMT_NOFILE format).
 * 最重要的是AVFormatContext 包含：
 * 是输入 或输出 格式。它可以是自动检测的，也可以是用户为输入设置的；输出总是由用户设置的。
 * AVStreams数组 ，描述文件中存储的所有基本流。AVStreams通常在此数组中使用其索引来指代。
 * 一个I/O上下文。它要么由lavf打开，要么由用户设置输入，总是由用户设置输出（除非您正在处理AVFMT_NOFILE格式）
 *
 * @section lavf_options Passing options to (de)muxers
 * It is possible to configure lavf muxers and demuxers using the @ref avoptions
 * mechanism. Generic (format-independent) libavformat options are provided by
 * AVFormatContext, they can be examined from a user program by calling
 * av_opt_next() / av_opt_find() on an allocated AVFormatContext (or its AVClass
 * from avformat_get_class()). Private (format-specific) options are provided by
 * AVFormatContext.priv_data if and only if AVInputFormat.priv_class /
 * AVOutputFormat.priv_class of the corresponding format struct is non-NULL.
 * Further options may be provided by the @ref AVFormatContext.pb "I/O context",
 * if its AVClass is non-NULL, and the protocols layer. See the discussion on
 * nesting in @ref avoptions documentation to learn how to access those.
 * 将选项传递给（解）复用器
 * 可以使用 AVOptions 机制配置 lavf 复用器和解复用器。 通用（与格式无关）libavformat 选项由 AVFormatContext
 * 提供，可以通过在分配的 AVFormatContext（或其来自 avformat_get_class() 的 AVClass）上调用 av_opt_next() / av_opt_find()
 * 从用户程序中检查它们。 当且仅当相应格式结构的 AVInputFormat.priv_class / AVOutputFormat.priv_class 为非 NULL 时，
 * AVFormatContext.priv_data 才会提供私有（特定于格式）选项。 如果 AVClass 为非 NULL，则 I/O 上下文和协议层可以提供更多选项。
 * 请参阅 AVOptions 文档中有关嵌套的讨论，了解如何访问它们。
 *
 * @section urls
 * URL strings in libavformat are made of a scheme/protocol, a ':', and a
 * scheme specific string. URLs without a scheme and ':' used for local files
 * are supported but deprecated. "file:" should be used for local files.
 *
 * It is important that the scheme string is not taken from untrusted
 * sources without checks.
 *
 * Note that some schemes/protocols are quite powerful, allowing access to
 * both local and remote files, parts of them, concatenations of them, local
 * audio and video devices and so on.
 * 网址
 * libavformat 中的 URL 字符串由schemes/协议、“:”和schemes特定字符串组成。 支持不带schemes的 URL 以及用于本地文件的“:”，
 * 但已弃用。 “file:”应用于本地文件。
 * 重要的是，schemes字符串不应未经检查就从不受信任的来源获取。
 * 请注意，某些schemes/协议非常强大，允许访问本地和远程文件、文件的一部分、文件的串联、本地音频和视频设备等。
 *
 * @{
 *
 * @defgroup lavf_decoding Demuxing
 * @{
 * Demuxers read a media file and split it into chunks of data (@em packets). A
 * @ref AVPacket "packet" contains one or more encoded frames which belongs to a
 * single elementary stream. In the lavf API this process is represented by the
 * avformat_open_input() function for opening a file, av_read_frame() for
 * reading a single packet and finally avformat_close_input(), which does the
 * cleanup.
 * 解复用器读取媒体文件并将其拆分为数据块（数据包）。一个数据包 包含一个或多个属于单个基本流的编码帧。
 * 在lavf中API这个过程由avformat_open_input（） 函数表示，用于打开文件，av_read_frame（） 用于读取单个数据包，
 * 最后是avformat_close_input（），用于清理。
 *
 * @section lavf_decoding_open Opening a media file
 * The minimum information required to open a file is its URL, which
 * is passed to avformat_open_input(), as in the following code:
 * 打开文件所需的最少信息是其 URL，该 URL 被传递给 avformat_open_input()，如以下代码所示：
 * @code
 * const char    *url = "file:in.mp3";
 * AVFormatContext *s = NULL;
 * int ret = avformat_open_input(&s, url, NULL, NULL);
 * if (ret < 0)
 *     abort();
 * @endcode
 * The above code attempts to allocate an AVFormatContext, open the
 * specified file (autodetecting the format) and read the header, exporting the
 * information stored there into s. Some formats do not have a header or do not
 * store enough information there, so it is recommended that you call the
 * avformat_find_stream_info() function which tries to read and decode a few
 * frames to find missing information.
 * 上面的代码尝试分配一个 AVFormatContext，打开指定的文件（自动检测格式）并读取标头，
 * 将其中存储的信息导出到 s 中。 某些格式没有标头或没有存储足够的信息，因此建议您调用
 * avformat_find_stream_info() 函数，该函数尝试读取和解码一些帧以查找丢失的信息。
 *
 * In some cases you might want to preallocate an AVFormatContext yourself with
 * avformat_alloc_context() and do some tweaking on it before passing it to
 * avformat_open_input(). One such case is when you want to use custom functions
 * for reading input data instead of lavf internal I/O layer.
 * To do that, create your own AVIOContext with avio_alloc_context(), passing
 * your reading callbacks to it. Then set the @em pb field of your
 * AVFormatContext to newly created AVIOContext.
 * 在某些情况下，您可能希望自己使用 avformat_alloc_context() 预先分配 AVFormatContext，
 * 并在将其传递给 avformat_open_input() 之前对其进行一些调整。 其中一种情况是当您想要使用自定义函数而
 * 不是 lavf 内部 I/O 层来读取输入数据时。 为此，请使用 avio_alloc_context() 创建您自己的 AVIOContext，
 * 并将您的读取回调传递给它。 然后将 AVFormatContext 的 pb 字段设置为新创建的 AVIOContext。
 *
 * Since the format of the opened file is in general not known until after
 * avformat_open_input() has returned, it is not possible to set demuxer private
 * options on a preallocated context. Instead, the options should be passed to
 * avformat_open_input() wrapped in an AVDictionary:
 * 由于打开的文件的格式通常要到avformat_open_input（）返回后才能知道，因此不可能在预分配的上下文中设置
 * demuxer私有选项。相反，选项应该传递给avformat_open_input（）包装在AVDictionary中：
 * @code
 * AVDictionary *options = NULL;
 * av_dict_set(&options, "video_size", "640x480", 0);
 * av_dict_set(&options, "pixel_format", "rgb24", 0);
 *
 * if (avformat_open_input(&s, url, NULL, &options) < 0)
 *     abort();
 * av_dict_free(&options);
 * @endcode
 * This code passes the private options 'video_size' and 'pixel_format' to the
 * demuxer. They would be necessary for e.g. the rawvideo demuxer, since it
 * cannot know how to interpret raw video data otherwise. If the format turns
 * out to be something different than raw video, those options will not be
 * recognized by the demuxer and therefore will not be applied. Such unrecognized
 * options are then returned in the options dictionary (recognized options are
 * consumed). The calling program can handle such unrecognized options as it
 * wishes, e.g.
 * 此代码将私有选项“video_size”和“pixel_format”传递给解复用器。它们对于例如rawvideo解复用器是必需的，
 * 因为它不知道如何解释原始视频数据。如果格式与原始视频不同，这些选项将不会被解复用器识别，因此不会应用。
 * 然后，这些无法识别的选项将在选项字典中返回（已识别的选项被使用）。调用程序可以根据需要处理这些无法识别的选项，例如。
 * @code
 * AVDictionaryEntry *e;
 * if (e = av_dict_get(options, "", NULL, AV_DICT_IGNORE_SUFFIX)) {
 *     fprintf(stderr, "Option %s not recognized by the demuxer.\n", e->key);
 *     abort();
 * }
 * @endcode
 *
 * After you have finished reading the file, you must close it with
 * avformat_close_input(). It will free everything associated with the file.
 * 读取完文件后，必须使用 avformat_close_input() 关闭它。 它将释放与该文件关联的所有内容。
 *
 * @section lavf_decoding_read Reading from an opened file
 * Reading data from an opened AVFormatContext is done by repeatedly calling
 * av_read_frame() on it. Each call, if successful, will return an AVPacket
 * containing encoded data for one AVStream, identified by
 * AVPacket.stream_index. This packet may be passed straight into the libavcodec
 * decoding functions avcodec_send_packet() or avcodec_decode_subtitle2() if the
 * caller wishes to decode the data.
 * 从打开的 AVFormatContext 中读取数据是通过重复调用 av_read_frame() 来完成的。 每次调用如果成功，
 * 将返回一个 AVPacket，其中包含一个 AVStream 的编码数据，由 AVPacket.stream_index 标识。
 * 如果调用者希望解码数据，则可以将该数据包直接传递到 libavcodec 解码函数 avcodec_send_packet()
 * 或 avcodec_decode_subtitle2() 中。
 *
 * AVPacket.pts, AVPacket.dts and AVPacket.duration timing information will be
 * set if known. They may also be unset (i.e. AV_NOPTS_VALUE for
 * pts/dts, 0 for duration) if the stream does not provide them. The timing
 * information will be in AVStream.time_base units, i.e. it has to be
 * multiplied by the timebase to convert them to seconds.
 *
 * 如果已知，将设置 AVPacket.pts、AVPacket.dts 和 AVPacket.duration 计时信息。 如果流不提供它们，
 * 它们也可能被取消设置（即 pts/dts 为 AV_NOPTS_VALUE，持续时间为 0）。 计时信息将以 AVStream.time_base
 * 为单位，即必须乘以时基才能将其转换为秒。
 *
 * A packet returned by av_read_frame() is always reference-counted,
 * i.e. AVPacket.buf is set and the user may keep it indefinitely.
 * The packet must be freed with av_packet_unref() when it is no
 * longer needed.
 *
 * av_read_frame()返回的数据包始终是引用计数的，即 AVPacket.buf 已设置并且用户可以无限期地保留它。
 * 当不再需要数据包时，必须使用 av_packet_unref() 释放它。
 *
 * @section lavf_decoding_seek Seeking
 * @}
 *
 * @defgroup lavf_encoding Muxing
 * @{
 * Muxers take encoded data in the form of @ref AVPacket "AVPackets" and write
 * it into files or other output bytestreams in the specified container format.
 * Muxers 以 AVPackets 形式获取编码数据，并将其写入指定容器格式的文件或其他输出字节流中。
 *
 * The main API functions for muxing are avformat_write_header() for writing the
 * file header, av_write_frame() / av_interleaved_write_frame() for writing the
 * packets and av_write_trailer() for finalizing the file.
 *
 * muxing的主要API函数是avformat_write_header()用于写入文件头，av_write_frame()/av_interleaved_write_frame()
 * 用于写入数据包和av_write_trailer（） 用于完成文件。
 *
 * At the beginning of the muxing process, the caller must first call
 * avformat_alloc_context() to create a muxing context. The caller then sets up
 * the muxer by filling the various fields in this context:
 * 在多路复用过程开始时，调用者必须首先调用 avformat_alloc_context() 创建多路复用上下文。 然后，
 * 调用者通过填写此上下文中的各个字段来设置复用器：
 *
 * - The @ref AVFormatContext.oformat "oformat" field must be set to select the
 *   muxer that will be used.
 *   必须设置 oformat 字段以选择将使用的复用器。
 *
 * - Unless the format is of the AVFMT_NOFILE type, the @ref AVFormatContext.pb
 *   "pb" field must be set to an opened IO context, either returned from
 *   avio_open2() or a custom one.
 *   除非格式是 AVFMT_NOFILE 类型，否则 pb 字段必须设置为打开的 IO 上下文，可以从 avio_open2() 返回，
 *   也可以是自定义的。
 *
 * - Unless the format is of the AVFMT_NOSTREAMS type, at least one stream must
 *   be created with the avformat_new_stream() function. The caller should fill
 *   the @ref AVStream.codecpar "stream codec parameters" information, such as the
 *   codec @ref AVCodecParameters.codec_type "type", @ref AVCodecParameters.codec_id
 *   "id" and other parameters (e.g. width / height, the pixel or sample format,
 *   etc.) as known. The @ref AVStream.time_base "stream timebase" should
 *   be set to the timebase that the caller desires to use for this stream (note
 *   that the timebase actually used by the muxer can be different, as will be
 *   described later).
 *   除非格式是 AVFMT_NOSTREAMS 类型，否则必须使用 avformat_new_stream() 函数至少创建一个流。
 *   调用者应填写已知的流编解码器参数信息，例如编解码器类型、id 和其他参数（例如宽度/高度、像素或样本格式等）。
 *   流时基应设置为调用者希望用于该流的时基（请注意，复用器实际使用的时基可以不同，如稍后所述）。
 *
 * - It is advised to manually initialize only the relevant fields in
 *   AVCodecParameters, rather than using @ref avcodec_parameters_copy() during
 *   remuxing: there is no guarantee that the codec context values remain valid
 *   for both input and output format contexts.
 *   建议仅手动初始化 AVCodecParameters 中的相关字段，而不是在重新复用期间使用 avcodec_parameters_copy()：
 *   不能保证编解码器上下文值对于输入和输出格式上下文都保持有效。
 *
 * - The caller may fill in additional information, such as @ref
 *   AVFormatContext.metadata "global" or @ref AVStream.metadata "per-stream"
 *   metadata, @ref AVFormatContext.chapters "chapters", @ref
 *   AVFormatContext.programs "programs", etc. as described in the
 *   AVFormatContext documentation. Whether such information will actually be
 *   stored in the output depends on what the container format and the muxer
 *   support.
 *   调用者可以填写附加信息，例如全局或每个流元数据、章节、程序等，如 AVFormatContext 文档中所述。
 *   这些信息是否实际存储在输出中取决于容器格式和复用器支持的内容。
 *
 * When the muxing context is fully set up, the caller must call
 * avformat_write_header() to initialize the muxer internals and write the file
 * header. Whether anything actually is written to the IO context at this step
 * depends on the muxer, but this function must always be called. Any muxer
 * private options must be passed in the options parameter to this function.
 * 当复用器上下文完全设置后，调用者必须调用 avformat_write_header() 来初始化复用器内部并写入文件头。
 * 在此步骤中是否实际将任何内容写入 IO 上下文取决于复用器，但必须始终调用此函数。 任何复用器私有选项都必须通过
 * options 参数传递给此函数。
 *
 * The data is then sent to the muxer by repeatedly calling av_write_frame() or
 * av_interleaved_write_frame() (consult those functions' documentation for
 * discussion on the difference between them; only one of them may be used with
 * a single muxing context, they should not be mixed). Do note that the timing
 * information on the packets sent to the muxer must be in the corresponding
 * AVStream's timebase. That timebase is set by the muxer (in the
 * avformat_write_header() step) and may be different from the timebase
 * requested by the caller.
 * 然后通过重复调用 av_write_frame() 或 av_interleaved_write_frame() 将数据发送到复用器（请参阅这些函数的文档以
 * 讨论它们之间的差异；只有其中一个可以与单个复用器上下文一起使用，它们不应混合使用 ）。 请注意，发送到复用器的数据包的
 * 计时信息必须位于相应 AVStream 的时基中。 该时基由复用器设置（在 avformat_write_header() 步骤中），并且可能与调用
 * 者请求的时基不同。
 *
 * Once all the data has been written, the caller must call av_write_trailer()
 * to flush any buffered packets and finalize the output file, then close the IO
 * context (if any) and finally free the muxing context with
 * avformat_free_context().
 * 一旦所有数据都被写入，调用者必须调用 av_write_trailer() 来刷新所有缓冲的数据包并最终确定输出文件，
 * 然后关闭 IO 上下文（如果有），最后使用 avformat_free_context() 释放多路复用上下文。
 * @}
 *
 * @defgroup lavf_io I/O Read/Write
 * @{
 * @section lavf_io_dirlist Directory listing
 * The directory listing API makes it possible to list files on remote servers.
 * 目录列表 API 可以列出远程服务器上的文件。
 *
 * Some of possible use cases:一些可能的用例：
 *
 * - an "open file" dialog to choose files from a remote location,
 * - a recursive media finder providing a player with an ability to play all
 * files from a given directory.
 * “打开文件”对话框用于从远程位置选择文件，
递归媒体查找器，为播放器提供播放给定目录中所有文件的能力。
 *
 * @subsection lavf_io_dirlist_open Opening a directory
 * 打开目录
 *
 * At first, a directory needs to be opened by calling avio_open_dir()
 * supplied with a URL and, optionally, ::AVDictionary containing
 * protocol-specific parameters. The function returns zero or positive
 * integer and allocates AVIODirContext on success.
 * 首先，需要通过调用 avio_open_dir() 打开一个目录，并提供 URL 和可选的包含协议特定参数的 AVDictionary。
 * 该函数返回零或正整数并在成功时分配 AVIODirContext。
 *
 * @code
 * AVIODirContext *ctx = NULL;
 * if (avio_open_dir(&ctx, "smb://example.com/some_dir", NULL) < 0) {
 *     fprintf(stderr, "Cannot open directory.\n");
 *     abort();
 * }
 * @endcode
 *
 * This code tries to open a sample directory using smb protocol without
 * any additional parameters.
 * 此代码尝试使用 smb 协议打开示例目录，无需任何其他参数。
 *
 * @subsection lavf_io_dirlist_read Reading entries
 * 读取条目
 *
 * Each directory's entry (i.e. file, another directory, anything else
 * within ::AVIODirEntryType) is represented by AVIODirEntry.
 * Reading consecutive entries from an opened AVIODirContext is done by
 * repeatedly calling avio_read_dir() on it. Each call returns zero or
 * positive integer if successful. Reading can be stopped right after the
 * NULL entry has been read -- it means there are no entries left to be
 * read. The following code reads all entries from a directory associated
 * with ctx and prints their names to standard output.
 * 每个目录的条目（即文件、另一个目录、AVIODirEntryType 中的任何其他内容）均由 AVIODirEntry 表示。
 * 从打开的 AVIODirContext 中读取连续条目是通过重复调用 avio_read_dir() 来完成的。 如果成功，
 * 每次调用都会返回零或正整数。 读取 NULL 条目后可以立即停止读取 – 这意味着没有剩余条目可供读取。
 * 以下代码从与 ctx 关联的目录中读取所有条目并将其名称打印到标准输出。
 *
 * @code
 * AVIODirEntry *entry = NULL;
 * for (;;) {
 *     if (avio_read_dir(ctx, &entry) < 0) {
 *         fprintf(stderr, "Cannot list directory.\n");
 *         abort();
 *     }
 *     if (!entry)
 *         break;
 *     printf("%s\n", entry->name);
 *     avio_free_directory_entry(&entry);
 * }
 * @endcode
 * @}
 *
 * @defgroup lavf_codec Demuxers
 * @{
 * @defgroup lavf_codec_native Native Demuxers
 * @{
 * @}
 * @defgroup lavf_codec_wrappers External library wrappers
 * @{
 * @}
 * @}
 * @defgroup lavf_protos I/O Protocols
 * @{
 * @}
 * @defgroup lavf_internal Internal
 * @{
 * @}
 * @}
 */

#include <time.h>
#include <stdio.h>  /* FILE */

#include "libavcodec/codec.h"
#include "libavcodec/codec_par.h"
#include "libavcodec/defs.h"
#include "libavcodec/packet.h"

#include "libavutil/dict.h"
#include "libavutil/log.h"

#include "avio.h"
#include "libavformat/version_major.h"
#ifndef HAVE_AV_CONFIG_H
/* When included as part of the ffmpeg build, only include the major version
 * to avoid unnecessary rebuilds. When included externally, keep including
 * the full version information. */
#include "libavformat/version.h"
#endif

struct AVFormatContext;

struct AVDeviceInfoList;

/**
 * @defgroup metadata_api Public Metadata API
 * 公共元数据API
 *
 * @{
 * @ingroup libavf
 * The metadata API allows libavformat to export metadata tags to a client
 * application when demuxing. Conversely it allows a client application to
 * set metadata when muxing.
 * 元数据 API 允许 libavformat 在解复用时将元数据标签导出到客户端应用程序。 相反，
 * 它允许客户端应用程序在复用时设置元数据。
 *
 * Metadata is exported or set as pairs of key/value strings in the 'metadata'
 * fields of the AVFormatContext, AVStream, AVChapter and AVProgram structs
 * using the @ref lavu_dict "AVDictionary" API. Like all strings in FFmpeg,
 * metadata is assumed to be UTF-8 encoded Unicode. Note that metadata
 * exported by demuxers isn't checked to be valid UTF-8 in most cases.
 * 使用 AVDictionary API 将元数据导出或设置为 AVFormatContext、AVStream、AVChater 和
 * AVProgram 结构的“元数据”字段中的键/值字符串对。 与 FFmpeg 中的所有字符串一样，
 * 元数据假定为 UTF-8 编码的 Unicode。 请注意，在大多数情况下，不会检查多路解复用器导出的元数据是否
 * 为有效的 UTF-8。
 *
 * Important concepts to keep in mind:
 * 要记住的重要概念：
 * -  Keys are unique; there can never be 2 tags with the same key. This is
 *    also meant semantically, i.e., a demuxer should not knowingly produce
 *    several keys that are literally different but semantically identical.
 *    E.g., key=Author5, key=Author6. In this example, all authors must be
 *    placed in the same tag.
 *    键是唯一的； 永远不可能有 2 个标签具有相同的键。 这也是语义上的意思，即，解复用器不应故意生成字面上
 *    不同但语义相同的多个键。 例如，key=Author5，key=Author6。 在此示例中，所有作者必须放置在同一标签中。
 *
 * -  Metadata is flat, not hierarchical; there are no subtags. If you
 *    want to store, e.g., the email address of the child of producer Alice
 *    and actor Bob, that could have key=alice_and_bobs_childs_email_address.
 *    元数据是扁平的，没有层次结构； 没有子标签。 例如，如果您想存储制片人 Alice 和演员 Bob 的孩子的电子邮件地址，
 *    则可以使用 key=alice_and_bobs_childs_email_address。
 *
 * -  Several modifiers can be applied to the tag name. This is done by
 *    appending a dash character ('-') and the modifier name in the order
 *    they appear in the list below -- e.g. foo-eng-sort, not foo-sort-eng.
 *    可以对标签名称应用多个修饰符。 这是通过按照下面列表中出现的顺序附加破折号字符（“-”）和修饰符名称来完成
 *    的 - 例如 foo-eng-sort，而不是 foo-sort-eng。
 *    -  language -- a tag whose value is localized for a particular language
 *       is appended with the ISO 639-2/B 3-letter language code.
 *       For example: Author-ger=Michael, Author-eng=Mike
 *       The original/default language is in the unqualified "Author" tag.
 *       A demuxer should set a default if it sets any translated tag.
 *       语言 – 其值针对特定语言进行本地化的标签，附加有 ISO 639-2/B 3 字母语言代码。 例如：
 *       Author-ger=Michael、Author-eng=Mike 原始/默认语言位于不合格的“Author”标签中。 如果解复用器设置任
 *       何翻译标签，则应设置默认值。
 *
 *    -  sorting  -- a modified version of a tag that should be used for
 *       sorting will have '-sort' appended. E.g. artist="The Beatles",
 *       artist-sort="Beatles, The".
 *       排序 – 用于排序的标签的修改版本将附加“-sort”。 例如。 艺术家=“披头士乐队”，艺术家排序=“披头士乐队”。
 *
 * - Some protocols and demuxers support metadata updates. After a successful
 *   call to av_read_frame(), AVFormatContext.event_flags or AVStream.event_flags
 *   will be updated to indicate if metadata changed. In order to detect metadata
 *   changes on a stream, you need to loop through all streams in the AVFormatContext
 *   and check their individual event_flags.
 *   一些协议和解复用器支持元数据更新。 成功调用 av_read_frame() 后，AVFormatContext.event_flags
 *   或 AVStream.event_flags 将更新以指示元数据是否更改。 为了检测流上的元数据更改，您需要循环遍历
 *   AVFormatContext 中的所有流并检查它们各自的 event_flags。
 *
 * -  Demuxers attempt to export metadata in a generic format, however tags
 *    with no generic equivalents are left as they are stored in the container.
 *    Follows a list of generic tag names:
 *    解复用器尝试以通用格式导出元数据，但是没有通用等效项的标签在存储在容器中时会被留下。 遵循通用标签名称列表：
 *
 @verbatim
 album        -- name of the set this work belongs to
 专辑 -- 该作品所属集合的名称

 album_artist -- main creator of the set/album, if different from artist.
                 e.g. "Various Artists" for compilation albums.
 专辑艺术家 -- 该系列/专辑的主要创作者（如果与艺术家不同）。例如 合辑专辑《Various Artists》。

 artist       -- main creator of the work
 艺术家——作品的主要创作者

 comment      -- any additional description of the file.
 注释——文件的任何附加描述。

 composer     -- who composed the work, if different from artist.
 作曲家——创作作品的人，如果与艺术家不同。

 copyright    -- name of copyright holder.
 版权——版权所有者的姓名。

 creation_time-- date when the file was created, preferably in ISO 8601.
 创建时间——创建文件的日期，最好采用 ISO 8601 格式。

 date         -- date when the work was created, preferably in ISO 8601.
 日期——作品创建的日期，最好采用 ISO 8601 格式。

 disc         -- number of a subset, e.g. disc in a multi-disc collection.
 光盘——子集的编号，例如 多光盘集合中的光盘。

 encoder      -- name/settings of the software/hardware that produced the file.
 编码器——生成文件的软件/硬件的名称/设置。

 encoded_by   -- person/group who created the file.
 编码者——创建该文件的人/组。

 filename     -- original name of the file.
 文件名——文件的原始名称。

 genre        -- <self-evident>.
 流派——<不言而喻的>。

 language     -- main language in which the work is performed, preferably
                 in ISO 639-2 format. Multiple languages can be specified by
                 separating them with commas.
 语言——展现作品的主要语言，最好采用 ISO 639-2 格式。 可以通过用逗号分隔来指定多种语言。

 performer    -- artist who performed the work, if different from artist.
                 E.g for "Also sprach Zarathustra", artist would be "Richard
                 Strauss" and performer "London Philharmonic Orchestra".
表演者——表演作品的艺术家，如果与艺术家不同。 例如，对于“Also sprach Zarathustra”，
艺术家是“理查德·施特劳斯”，表演者是“伦敦爱乐乐团”。

 publisher    -- name of the label/publisher.
 出版商——唱片公司/出版商的名称。

 service_name     -- name of the service in broadcasting (channel name).
 service_name——广播中的服务名称（频道名称）。

 service_provider -- name of the service provider in broadcasting.
 service_provider——广播中服务提供商的名称。

 title        -- name of the work.
 标题——作品的名称。

 track        -- number of this work in the set, can be in form current/total.
 track——该作品在该集中的编号，可以采用当前/总计的形式。

 variant_bitrate -- the total bitrate of the bitrate variant that the current stream is part of
 variant_bitrate——当前流所属的比特率变体的总比特率

 @endverbatim
 *
 * Look in the examples section for an application example how to use the Metadata API.
 * 查看示例部分，了解如何使用元数据 API 的应用程序示例。
 *
 * @}
 */

/* packet functions */


/**
 * Allocate and read the payload of a packet and initialize its
 * fields with default values.
 * 分配并读取数据包的有效负载，并使用默认值初始化其字段。
 *
 * @param s    associated IO context
 * @param pkt packet
 * @param size desired payload size
 * @return >0 (read size) if OK, AVERROR_xxx otherwise
 */
int av_get_packet(AVIOContext *s, AVPacket *pkt, int size);


/**
 * Read data and append it to the current content of the AVPacket.
 * 读取数据并将其附加到 AVPacket 的当前内容中。
 *
 * If pkt->size is 0 this is identical to av_get_packet.
 * Note that this uses av_grow_packet and thus involves a realloc
 * which is inefficient. Thus this function should only be used
 * when there is no reasonable way to know (an upper bound of)
 * the final size.
 * 如果 pkt->size 为 0，则与 av_get_packet 相同。 请注意，这使用了 av_grow_packet，
 * 因此涉及低效的重新分配。 因此，仅当没有合理的方法来知道最终大小（的上限）时才应使用此函数。
 *
 * @param s    associated IO context
 * @param pkt packet
 * @param size amount of data to read
 * @return >0 (read size) if OK, AVERROR_xxx otherwise, previous data
 *         will not be lost even if an error occurs.
 */
int av_append_packet(AVIOContext *s, AVPacket *pkt, int size);

/*************************************************/
/* input/output formats */

struct AVCodecTag;

/**
 * This structure contains the data a format has to probe a file.
 * 此结构包含格式探测文件所需的数据。
 */
typedef struct AVProbeData {
    const char *filename;
    unsigned char *buf; /**< Buffer must have AVPROBE_PADDING_SIZE of extra allocated bytes filled with zero.
                         缓冲区必须具有 AVPROBE_PADDING_SIZE 的额外分配字节，并用零填充。
                         */
    int buf_size;       /**< Size of buf except extra allocated bytes */
    const char *mime_type; /**< mime_type, when known. */
} AVProbeData;

#define AVPROBE_SCORE_RETRY (AVPROBE_SCORE_MAX/4)
#define AVPROBE_SCORE_STREAM_RETRY (AVPROBE_SCORE_MAX/4-1)

#define AVPROBE_SCORE_EXTENSION  50 ///< score for file extension
#define AVPROBE_SCORE_MIME       75 ///< score for file mime type
#define AVPROBE_SCORE_MAX       100 ///< maximum score

#define AVPROBE_PADDING_SIZE 32             ///< extra allocated bytes at the end of the probe buffer

/// Demuxer will use avio_open, no opened file should be provided by the caller.
/// Demuxer 将使用 avio_open，调用者不应提供打开的文件。
#define AVFMT_NOFILE        0x0001
#define AVFMT_NEEDNUMBER    0x0002 /**< Needs '%d' in filename. */
/**
 * The muxer/demuxer is experimental and should be used with caution.
 * 复用器/解复用器是实验性的，应谨慎使用。
 *
 * - demuxers: will not be selected automatically by probing, must be specified
 *             explicitly.
 * 解复用器：不会通过探测自动选择，必须明确指定。
 */
#define AVFMT_EXPERIMENTAL  0x0004
#define AVFMT_SHOW_IDS      0x0008 /**< Show format stream IDs numbers. */
#define AVFMT_GLOBALHEADER  0x0040 /**< Format wants global header. */
#define AVFMT_NOTIMESTAMPS  0x0080 /**< Format does not need / have any timestamps. */
#define AVFMT_GENERIC_INDEX 0x0100 /**< Use generic index building code.使用通用索引构建代码。 */
#define AVFMT_TS_DISCONT    0x0200 /**< Format allows timestamp discontinuities. Note, muxers always require valid (monotone) timestamps
                                    格式允许时间戳不连续。 请注意，复用器始终需要有效（单调）时间戳 */
#define AVFMT_VARIABLE_FPS  0x0400 /**< Format allows variable fps. */
#define AVFMT_NODIMENSIONS  0x0800 /**< Format does not need width/height */
#define AVFMT_NOSTREAMS     0x1000 /**< Format does not require any streams */
#define AVFMT_NOBINSEARCH   0x2000 /**< Format does not allow to fall back on binary search via read_timestamp
                                    格式不允许通过 read_timestamp 进行二分搜索*/
#define AVFMT_NOGENSEARCH   0x4000 /**< Format does not allow to fall back on generic search */
#define AVFMT_NO_BYTE_SEEK  0x8000 /**< Format does not allow seeking by bytes 格式不允许依靠通用搜索 */
#define AVFMT_ALLOW_FLUSH  0x10000 /**< Format allows flushing. If not set, the muxer will not receive a NULL packet in the write_packet function.
                                    格式允许冲洗。 如果未设置，复用器将不会在 write_packet 函数中接收 NULL 数据包。*/
#define AVFMT_TS_NONSTRICT 0x20000 /**< Format does not require strictly
                                        increasing timestamps, but they must
                                        still be monotonic 格式不要求严格增加时间戳，但它们仍然必须是单调的 */
#define AVFMT_TS_NEGATIVE  0x40000 /**< Format allows muxing negative
                                        timestamps. If not set the timestamp
                                        will be shifted in av_write_frame and
                                        av_interleaved_write_frame so they
                                        start from 0.
                                        The user or muxer can override this through
                                        AVFormatContext.avoid_negative_ts
                                        格式允许混合负时间戳。 如果未设置，时间戳将在 av_write_frame 和
                                        av_interleaved_write_frame 中移动，因此它们从 0 开始。
                                        用户或复用器可以通过 AVFormatContext.avoid_negative_ts 覆盖它
                                        */

#define AVFMT_SEEK_TO_PTS   0x4000000 /**< Seeking is based on PTS */

/**
 * @addtogroup lavf_encoding
 * @{
 */
typedef struct AVOutputFormat {
    const char *name;
    /**
     * Descriptive name for the format, meant to be more human-readable
     * than name. You should use the NULL_IF_CONFIG_SMALL() macro
     * to define it.
     * 格式的描述性名称，比名称更易于理解。 您应该使用 NULL_IF_CONFIG_SMALL() 宏来定义它。
     */
    const char *long_name;
    const char *mime_type;
    const char *extensions; /**< comma-separated filename extensions
                             以逗号分隔的文件扩展名 */
    /* output support */
    enum AVCodecID audio_codec;    /**< default audio codec */
    enum AVCodecID video_codec;    /**< default video codec */
    enum AVCodecID subtitle_codec; /**< default subtitle codec */
    /**
     * can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER,
     * AVFMT_GLOBALHEADER, AVFMT_NOTIMESTAMPS, AVFMT_VARIABLE_FPS,
     * AVFMT_NODIMENSIONS, AVFMT_NOSTREAMS, AVFMT_ALLOW_FLUSH,
     * AVFMT_TS_NONSTRICT, AVFMT_TS_NEGATIVE
     */
    int flags;

    /**
     * List of supported codec_id-codec_tag pairs, ordered by "better
     * choice first". The arrays are all terminated by AV_CODEC_ID_NONE.
     * 支持的 codec_id-codec_tag 对列表，按“更好的选择优先”排序。 数组均以 AV_CODEC_ID_NONE 终止。
     */
    const struct AVCodecTag * const *codec_tag;


    const AVClass *priv_class; ///< AVClass for the private context
} AVOutputFormat;
/**
 * @}
 */

/**
 * @addtogroup lavf_decoding
 * @{
 */
typedef struct AVInputFormat {
    /**
     * A comma separated list of short names for the format. New names
     * may be appended with a minor bump.
     * 以逗号分隔的格式短名称列表。 新名称可能会附加一个小凸起。
     */
    const char *name;

    /**
     * Descriptive name for the format, meant to be more human-readable
     * than name. You should use the NULL_IF_CONFIG_SMALL() macro
     * to define it.
     * 格式的描述性名称，比名称更易于理解。 您应该使用 NULL_IF_CONFIG_SMALL() 宏来定义它。
     */
    const char *long_name;

    /**
     * Can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER, AVFMT_SHOW_IDS,
     * AVFMT_NOTIMESTAMPS, AVFMT_GENERIC_INDEX, AVFMT_TS_DISCONT, AVFMT_NOBINSEARCH,
     * AVFMT_NOGENSEARCH, AVFMT_NO_BYTE_SEEK, AVFMT_SEEK_TO_PTS.
     */
    int flags;

    /**
     * If extensions are defined, then no probe is done. You should
     * usually not use extension format guessing because it is not
     * reliable enough
     * 如果定义了扩展，则不会进行任何探测。 您通常不应该使用扩展格式猜测，因为它不够可靠
     */
    const char *extensions;

    const struct AVCodecTag * const *codec_tag;

    const AVClass *priv_class; ///< AVClass for the private context

    /**
     * Comma-separated list of mime types.
     * It is used check for matching mime types while probing.
     * @see av_probe_input_format2
     * 以逗号分隔的 MIME 类型列表。 它用于在探测时检查匹配的 mime 类型。 @参见 av_probe_input_format2
     */
    const char *mime_type;

    /*****************************************************************
     * No fields below this line are part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     * 此行以下的任何字段都不属于公共 API 的一部分。 它们不能在 libavformat 之外使用，
     * 并且可以随意更改和删除。新的公共字段应添加到正上方。
     *****************************************************************
     */
    /**
     * Raw demuxers store their codec ID here.
     * 原始解复用器将其编解码器 ID 存储在此处
     */
    int raw_codec_id;

    /**
     * Size of private data so that it can be allocated in the wrapper.
     * 私有数据的大小，以便可以在包装器中分配它。
     */
    int priv_data_size;

    /**
     * Internal flags. See FF_FMT_FLAG_* in internal.h.
     * 内部标志。 请参阅internal.h 中的FF_FMT_FLAG_*。
     */
    int flags_internal;

    /**
     * Tell if a given file has a chance of being parsed as this format.
     * The buffer provided is guaranteed to be AVPROBE_PADDING_SIZE bytes
     * big so you do not have to check for that unless you need more.
     * 判断给定文件是否有机会被解析为这种格式。 提供的缓冲区保证为 AVPROBE_PADDING_SIZE 字节大，
     * 因此您不必检查它，除非您需要更多。
     */
    int (*read_probe)(const AVProbeData *);

    /**
     * Read the format header and initialize the AVFormatContext
     * structure. Return 0 if OK. 'avformat_new_stream' should be
     * called to create new streams.
     * 读取格式头并初始化AVFormatContext结构。 如果成功则返回 0。 应调用“avformat_new_stream”来创建新流。
     */
    int (*read_header)(struct AVFormatContext *);

    /**
     * Read one packet and put it in 'pkt'. pts and flags are also
     * set. 'avformat_new_stream' can be called only if the flag
     * AVFMTCTX_NOHEADER is used and only in the calling thread (not in a
     * background thread).
     * 读取一个数据包并将其放入“pkt”中。 还设置了 pts 和 flag。 仅当使用标志 AVFMTCTX_NOHEADER 且仅
     * 在调用线程中（不在后台线程中）才能调用“avformat_new_stream”。
     * @return 0 on success, < 0 on error.
     *         Upon returning an error, pkt must be unreferenced by the caller.
     */
    int (*read_packet)(struct AVFormatContext *, AVPacket *pkt);

    /**
     * Close the stream. The AVFormatContext and AVStreams are not
     * freed by this function
     * 关闭流。 该函数不会释放 AVFormatContext 和 AVStreams
     */
    int (*read_close)(struct AVFormatContext *);

    /**
     * Seek to a given timestamp relative to the frames in
     * stream component stream_index.
     * 查找相对于流组件stream_index 中的帧的给定时间戳。
     * @param stream_index Must not be -1.
     * @param flags Selects which direction should be preferred if no exact
     *              match is available. 如果没有精确匹配可用，则选择应首选的方向。
     * @return >= 0 on success (but not necessarily the new offset)
     */
    int (*read_seek)(struct AVFormatContext *,
                     int stream_index, int64_t timestamp, int flags);

    /**
     * Get the next timestamp in stream[stream_index].time_base units.
     * 获取以stream[stream_index].time_base为单位的下一个时间戳。
     * @return the timestamp or AV_NOPTS_VALUE if an error occurred
     */
    int64_t (*read_timestamp)(struct AVFormatContext *s, int stream_index,
                              int64_t *pos, int64_t pos_limit);

    /**
     * Start/resume playing - only meaningful if using a network-based format
     * (RTSP).
     * 开始/恢复播放 - 仅在使用基于网络的格式 (RTSP) 时才有意义。
     */
    int (*read_play)(struct AVFormatContext *);

    /**
     * Pause playing - only meaningful if using a network-based format
     * (RTSP).
     * 暂停播放 - 仅在使用基于网络的格式 (RTSP) 时才有意义。
     */
    int (*read_pause)(struct AVFormatContext *);

    /**
     * Seek to timestamp ts.
     * Seeking will be done so that the point from which all active streams
     * can be presented successfully will be closest to ts and within min/max_ts.
     * Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
     * seek到时间戳 ts。 将进行查找，以便可以成功呈现所有活动流的点将最接近 ts 并在 min/max_ts 之内。
     * 活动流是 AVStream.discard < AVDISCARD_ALL 的所有流。
     */
    int (*read_seek2)(struct AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags);

    /**
     * Returns device list with it properties.
     * 返回设备列表及其属性。
     * @see avdevice_list_devices() for more details.
     */
    int (*get_device_list)(struct AVFormatContext *s, struct AVDeviceInfoList *device_list);

} AVInputFormat;
/**
 * @}
 */

enum AVStreamParseType {
    AVSTREAM_PARSE_NONE,
    AVSTREAM_PARSE_FULL,       /**< full parsing and repack
                                完整解析和重新打包 */
    AVSTREAM_PARSE_HEADERS,    /**< Only parse headers, do not repack.
                                只解析头部，不重新打包。*/
    AVSTREAM_PARSE_TIMESTAMPS, /**< full parsing and interpolation of timestamps for frames not starting on a packet boundary
                                对不从数据包边界开始的帧进行完整解析和时间戳插值 */
    AVSTREAM_PARSE_FULL_ONCE,  /**< full parsing and repack of the first frame only, only implemented for H.264 currently
                                仅第一帧的完整解析和重新打包，目前仅针对 H.264 实现 */
    AVSTREAM_PARSE_FULL_RAW,   /**< full parsing and repack with timestamp and position generation by parser for raw
                                    this assumes that each packet in the file contains no demuxer level headers and
                                    just codec level data, otherwise position generation would fail
                                    完整解析和重新打包，并由解析器为原始解析器生成时间戳和位置，这假设文件中的每个数据包不包
                                    含解复用器级别标头，仅包含编解码器级别数据，否则位置生成将失败 */
};

typedef struct AVIndexEntry {
    int64_t pos;
    int64_t timestamp;        /**<
                               * Timestamp in AVStream.time_base units, preferably the time from which on correctly decoded frames are available
                               * when seeking to this entry. That means preferable PTS on keyframe based formats.
                               * But demuxers can choose to store a different timestamp, if it is more convenient for the implementation or nothing better
                               * is known
                               * 以 AVStream.time_base 为单位的时间戳，最好是在查找此条目时正确解码的帧可用的时间。
                               * 这意味着基于关键帧的格式更适合 PTS。
                               * 但解复用器可以选择存储不同的时间戳，如果它更方便实现或者没有更好的已知信息
                               */
#define AVINDEX_KEYFRAME 0x0001
#define AVINDEX_DISCARD_FRAME  0x0002    /**
                                          * Flag is used to indicate which frame should be discarded after decoding.
                                          */
    int flags:2;
    int size:30; /*Yeah, trying to keep the size of this small to reduce memory requirements (it is 24 vs. 32 bytes due to possible 8-byte alignment).
                   是的，尝试保持较小的大小以减少内存需求（由于可能的 8 字节对齐，它是 24 与 32 字节）。*/
    int min_distance;         /**< Minimum distance between this and the previous keyframe, used to avoid unneeded searching.
                               此关键帧与前一个关键帧之间的最小距离，用于避免不必要的搜索。*/
}   AVIndexEntry;

/**
 * The stream should be chosen by default among other streams of the same type,
 * unless the user has explicitly specified otherwise.
 * 除非用户明确指定，否则应默认从相同类型的其他流中选择该流。
 */
#define AV_DISPOSITION_DEFAULT              (1 << 0)
/**
 * The stream is not in original language.
 * 该流不是原始语言。
 *
 * @note AV_DISPOSITION_ORIGINAL is the inverse of this disposition. At most
 *       one of them should be set in properly tagged streams.
 * @note This disposition may apply to any stream type, not just audio.
 * AV_DISPOSITION_ORIGINAL 是此配置的逆过程。 最多应将其中之一设置在正确标记的流中。
 * 此配置可以适用于任何流类型，而不仅仅是音频。
 */
#define AV_DISPOSITION_DUB                  (1 << 1)
/**
 * The stream is in original language.
 * 该流为原始语言。
 *
 * @see the notes for AV_DISPOSITION_DUB
 */
#define AV_DISPOSITION_ORIGINAL             (1 << 2)
/**
 * The stream is a commentary track.
 * 该流是评论曲目。
 */
#define AV_DISPOSITION_COMMENT              (1 << 3)
/**
 * The stream contains song lyrics.
 * 该流包含歌词。
 */
#define AV_DISPOSITION_LYRICS               (1 << 4)
/**
 * The stream contains karaoke audio.
 * 该流包含卡拉 OK 音频。
 */
#define AV_DISPOSITION_KARAOKE              (1 << 5)

/**
 * Track should be used during playback by default.
 * Useful for subtitle track that should be displayed
 * even when user did not explicitly ask for subtitles.
 * 默认情况下应在播放期间使用轨道。
 * 对于即使用户没有明确要求字幕也应该显示的字幕轨道很有用。
 */
#define AV_DISPOSITION_FORCED               (1 << 6)
/**
 * The stream is intended for hearing impaired audiences.
 * 该直播专为听力受损的观众而设计。
 */
#define AV_DISPOSITION_HEARING_IMPAIRED     (1 << 7)
/**
 * The stream is intended for visually impaired audiences.
 * 该直播面向视障观众。
 */
#define AV_DISPOSITION_VISUAL_IMPAIRED      (1 << 8)
/**
 * The audio stream contains music and sound effects without voice.
 * 音频流包含音乐和音效，但不包含语音。
 */
#define AV_DISPOSITION_CLEAN_EFFECTS        (1 << 9)
/**
 * The stream is stored in the file as an attached picture/"cover art" (e.g.
 * APIC frame in ID3v2). The first (usually only) packet associated with it
 * will be returned among the first few packets read from the file unless
 * seeking takes place. It can also be accessed at any time in
 * AVStream.attached_pic.
 */
#define AV_DISPOSITION_ATTACHED_PIC         (1 << 10)
/**
 * The stream is sparse, and contains thumbnail images, often corresponding
 * to chapter markers. Only ever used with AV_DISPOSITION_ATTACHED_PIC.
 * 该流是稀疏的，并且包含缩略图，通常对应于章节标记。 仅与 AV_DISPOSITION_ATTACHED_PIC 一起使用。
 */
#define AV_DISPOSITION_TIMED_THUMBNAILS     (1 << 11)

/**
 * The stream is intended to be mixed with a spatial audio track. For example,
 * it could be used for narration or stereo music, and may remain unchanged by
 * listener head rotation.
 * 该流旨在与空间音轨混合。 例如，它可以用于旁白或立体声音乐，并且可以通过听众头部旋转而保持不变。
 */
#define AV_DISPOSITION_NON_DIEGETIC         (1 << 12)

/**
 * The subtitle stream contains captions, providing a transcription and possibly
 * a translation of audio. Typically intended for hearing-impaired audiences.
 * 字幕流包含字幕，提供转录以及可能的音频翻译。 通常面向有听力障碍的观众。
 */
#define AV_DISPOSITION_CAPTIONS             (1 << 16)
/**
 * The subtitle stream contains a textual description of the video content.
 * Typically intended for visually-impaired audiences or for the cases where the
 * video cannot be seen.
 * 字幕流包含视频内容的文本描述。 通常适用于视障观众或无法观看视频的情况。
 */
#define AV_DISPOSITION_DESCRIPTIONS         (1 << 17)
/**
 * The subtitle stream contains time-aligned metadata that is not intended to be
 * directly presented to the user.
 * 字幕流包含不打算直接呈现给用户的时间对齐元数据。
 */
#define AV_DISPOSITION_METADATA             (1 << 18)
/**
 * The audio stream is intended to be mixed with another stream before
 * presentation.
 * Corresponds to mix_type=0 in mpegts.
 * 音频流旨在在呈现之前与另一个流混合。对应于mpegts中的mix_type=0。
 */
#define AV_DISPOSITION_DEPENDENT            (1 << 19)
/**
 * The video stream contains still images.
 * 视频流包含静止图像。
 */
#define AV_DISPOSITION_STILL_IMAGE          (1 << 20)

/**
 * @return The AV_DISPOSITION_* flag corresponding to disp or a negative error
 *         code if disp does not correspond to a known stream disposition.
 * AV_DISPOSITION_* 标志对应于 disp 或负错误代码，如果 disp 不对应于已知的流配置。
 */
int av_disposition_from_string(const char *disp);

/**
 * @param disposition a combination of AV_DISPOSITION_* values
 * AV_DISPOSITION_* 值的组合
 * @return The string description corresponding to the lowest set bit in
 *         disposition. NULL when the lowest set bit does not correspond
 *         to a known disposition or when disposition is 0.
 * 与配置中最低设置位相对应的字符串描述。 当最低设置位不对应于已知配置或当配置为 0 时，为 NULL。
 */
const char *av_disposition_to_string(int disposition);

/**
 * Options for behavior on timestamp wrap detection.
 * 时间戳回绕检测的行为选项。
 */
#define AV_PTS_WRAP_IGNORE      0   ///< ignore the wrap
#define AV_PTS_WRAP_ADD_OFFSET  1   ///< add the format specific offset on wrap detection，在换行检测上添加特定于格式的偏移量
#define AV_PTS_WRAP_SUB_OFFSET  -1  ///< subtract the format specific offset on wrap detection，减去换行检测时格式特定的偏移量

/**
 * Stream structure.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVStream) must not be used outside libav*.
 * 流结构。
 * 新字段可以添加到末尾并进行较小的版本更新。 对现有字段的删除、重新排序和更改需要主要版本更新。
 * sizeof(AVStream) 不得在 libav* 之外使用。
 *
 */
typedef struct AVStream {
    /**
     * A class for @ref avoptions. Set on stream creation.
     */
    const AVClass *av_class;

    int index;    /**< stream index in AVFormatContext */
    /**
     * Format-specific stream ID.
     * decoding: set by libavformat
     * encoding: set by the user, replaced by libavformat if left unset
     */
    int id;

    /**
     * Codec parameters associated with this stream. Allocated and freed by
     * libavformat in avformat_new_stream() and avformat_free_context()
     * respectively.
     *
     * - demuxing: filled by libavformat on stream creation or in
     *             avformat_find_stream_info()
     * - muxing: filled by the caller before avformat_write_header()
     * 与该流关联的编解码器参数。
     * 由 libavformat 分别在 avformat_new_stream() 和 avformat_free_context() 中分配和释放。
     * 解复用：在流创建时或 avformat_find_stream_info() 中由 libavformat 填充
     * 复用：由调用者在 avformat_write_header() 之前填充
     */
    AVCodecParameters *codecpar;

    void *priv_data;

    /**
     * This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented.
     *
     * decoding: set by libavformat
     * encoding: May be set by the caller before avformat_write_header() to
     *           provide a hint to the muxer about the desired timebase. In
     *           avformat_write_header(), the muxer will overwrite this field
     *           with the timebase that will actually be used for the timestamps
     *           written into the file (which may or may not be related to the
     *           user-provided one, depending on the format).
     * 这是表示帧时间戳的基本时间单位（以秒为单位）。
     * 解码：由libavformat设置
     * 编码：可以由调用者在 avformat_write_header() 之前设置，以向复用器提供有关所需时基
     * 的提示。 在 avformat_write_header() 中，复用器将使用实际用于写入文件的时间戳的时基覆盖
     * 此字段（该时基可能与用户提供的时间戳相关，也可能无关，具体取决于格式）。
     */
    AVRational time_base;

    /**
     * Decoding: pts of the first frame of the stream in presentation order, in stream time base.
     * Only set this if you are absolutely 100% sure that the value you set
     * it to really is the pts of the first frame.
     * This may be undefined (AV_NOPTS_VALUE).
     * @note The ASF header does NOT contain a correct start_time the ASF
     * demuxer must NOT set this.
     * 解码：流时基中按呈现顺序排列的流第一帧的pts。
     * 仅当您绝对 100% 确定您设置的值确实是第一帧的 pts 时才设置此值。 这可能是未定义的（AV_NOPTS_VALUE）。
     */
    int64_t start_time;

    /**
     * Decoding: duration of the stream, in stream time base.
     * If a source file does not specify a duration, but does specify
     * a bitrate, this value will be estimated from bitrate and file size.
     *
     * Encoding: May be set by the caller before avformat_write_header() to
     * provide a hint to the muxer about the estimated duration.
     * 解码：流的持续时间，以流时基为单位。
     * 如果源文件未指定持续时间，但指定了比特率，则将根据比特率和文件大小估计该值。
     * 编码：可以由调用者在 avformat_write_header() 之前设置，以向复用器提供有关估计持续时间的提示。
     */
    int64_t duration;

    int64_t nb_frames;                 ///< number of frames in this stream if known or 0，该流中的帧数（如果已知）或 0

    /**
     * Stream disposition - a combination of AV_DISPOSITION_* flags.
     * - demuxing: set by libavformat when creating the stream or in
     *             avformat_find_stream_info().
     * - muxing: may be set by the caller before avformat_write_header().
     * 流配置 - AV_DISPOSITION_* 标志的组合。
     * 解复用：在创建流时或在 avformat_find_stream_info() 中由 libavformat 设置。
     * 复用：可以由调用者在 avformat_write_header() 之前设置。
     */
    int disposition;

    enum AVDiscard discard; ///< Selects which packets can be discarded at will and do not need to be demuxed.选择哪些数据包可以随意丢弃并且不需要解复用。

    /**
     * sample aspect ratio (0 if unknown)
     * 样本纵横比（如果未知则为 0）
     * - encoding: Set by user.
     * - decoding: Set by libavformat.
     */
    AVRational sample_aspect_ratio;

    AVDictionary *metadata;

    /**
     * Average framerate
     * 平均帧率
     *
     * - demuxing: May be set by libavformat when creating the stream or in
     *             avformat_find_stream_info().
     * - muxing: May be set by the caller before avformat_write_header().
     */
    AVRational avg_frame_rate;

    /**
     * For streams with AV_DISPOSITION_ATTACHED_PIC disposition, this packet
     * will contain the attached picture.
     *
     * decoding: set by libavformat, must not be modified by the caller.
     * encoding: unused
     * 对于具有 AV_DISPOSITION_ATTACHED_PIC 配置的流，此数据包将包含附加图片。
     * 解码：由 libavformat 设置，调用者不得修改。
     * 编码：未使用
     */
    AVPacket attached_pic;

    /**
     * An array of side data that applies to the whole stream (i.e. the
     * container does not allow it to change between packets).
     * 适用于整个流的辅助数据数组（即容器不允许它在数据包之间更改）。
     *
     * There may be no overlap between the side data in this array and side data
     * in the packets. I.e. a given side data is either exported by the muxer
     * (demuxing) / set by the caller (muxing) in this array, then it never
     * appears in the packets, or the side data is exported / sent through
     * the packets (always in the first packet where the value becomes known or
     * changes), then it does not appear in this array.
     * 该数组中的辅助数据和数据包中的辅助数据之间可能没有重叠。
     * 即给定的辅助数据要么由复用器导出（解复用）/由调用者设置（复用）在此数组中，
     * 然后它永远不会出现在数据包中，要么通过数据包导出/发送辅助数据（始终在第一个数据包中） 如果该值已知或发生变化），
     * 则它不会出现在该数组中。
     *
     * - demuxing: Set by libavformat when the stream is created.
     * - muxing: May be set by the caller before avformat_write_header().
     *
     * Freed by libavformat in avformat_free_context().
     * 解复用：创建流时由 libavformat 设置。
     * 复用：可以由调用者在 avformat_write_header() 之前设置。
     * 由 avformat_free_context() 中的 libavformat 释放。
     *
     * @see av_format_inject_global_side_data()
     */
    AVPacketSideData *side_data;
    /**
     * The number of elements in the AVStream.side_data array.
     * AVStream.side_data 数组中的元素数量
     */
    int            nb_side_data;

    /**
     * Flags indicating events happening on the stream, a combination of
     * AVSTREAM_EVENT_FLAG_*.
     * 指示流上发生的事件的标志，AVSTREAM_EVENT_FLAG_* 的组合。
     *
     * - demuxing: may be set by the demuxer in avformat_open_input(),
     *   avformat_find_stream_info() and av_read_frame(). Flags must be cleared
     *   by the user once the event has been handled.
     *   解复用：可以由解复用器在 avformat_open_input()、avformat_find_stream_info() 和
     *   av_read_frame() 中设置。 处理事件后，用户必须清除标志。
     * - muxing: may be set by the user after avformat_write_header(). to
     *   indicate a user-triggered event.  The muxer will clear the flags for
     *   events it has handled in av_[interleaved]_write_frame().
     *   复用：可以由用户在 avformat_write_header() 之后设置。 指示用户触发的事件。
     *   复用器将清除它在 av_[interleaved]_write_frame() 中处理的事件的标志。
     */
    int event_flags;
/**
 * - demuxing: the demuxer read new metadata from the file and updated
 *     AVStream.metadata accordingly
 *   解复用器：解复用器从文件中读取新的元数据并相应地更新 AVStream.metadata
 * - muxing: the user updated AVStream.metadata and wishes the muxer to write
 *     it into the file
 *   复用：用户更新了 AVStream.metadata 并希望 muxer 将其写入文件
 */
#define AVSTREAM_EVENT_FLAG_METADATA_UPDATED 0x0001
/**
 * - demuxing: new packets for this stream were read from the file. This
 *   event is informational only and does not guarantee that new packets
 *   for this stream will necessarily be returned from av_read_frame().
 *   解复用：从文件中读取该流的新数据包。 此事件仅供参考，并不保证该流的新数据包一定会从 av_read_frame() 返回。
 */
#define AVSTREAM_EVENT_FLAG_NEW_PACKETS (1 << 1)

    /**
     * Real base framerate of the stream.
     * This is the lowest framerate with which all timestamps can be
     * represented accurately (it is the least common multiple of all
     * framerates in the stream). Note, this value is just a guess!
     * For example, if the time base is 1/90000 and all frames have either
     * approximately 3600 or 1800 timer ticks, then r_frame_rate will be 50/1.
     * 流的实际基本帧速率。
     * 这是可以准确表示所有时间戳的最低帧速率（它是流中所有帧速率的最小公倍数）。
     * 请注意，该值只是一个猜测！ 例如，如果时基为 1/90000，并且所有帧具有大约 3600 或 1800
     * 个计时器滴答，则 r_frame_rate 将为 50/1。
     */
    AVRational r_frame_rate;

    /**
     * Number of bits in timestamps. Used for wrapping control.
     * 时间戳中的位数。 用于包裹控制。
     *
     * - demuxing: set by libavformat
     * - muxing: set by libavformat
     *
     */
    int pts_wrap_bits;
} AVStream;

struct AVCodecParserContext *av_stream_get_parser(const AVStream *s);

#if FF_API_GET_END_PTS
/**
 * Returns the pts of the last muxed packet + its duration
 *
 * the retuned value is undefined when used with a demuxer.
 * 返回最后一个复用数据包的pts + 其持续时间
 * 与解复用器一起使用时，返回的值是未定义的。
 */
attribute_deprecated
int64_t    av_stream_get_end_pts(const AVStream *st);
#endif

#define AV_PROGRAM_RUNNING 1

/**
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVProgram) must not be used outside libav*.
 * 新字段可以通过次要版本提升添加到末尾。删除、重新排序和更改现有字段需要主要版本提升。
 * sizeof(AVProgram) 不得在 libav* 之外使用。
 */
typedef struct AVProgram {
    int            id;
    int            flags;
    enum AVDiscard discard;        ///< selects which program to discard and which to feed to the caller
    unsigned int   *stream_index;
    unsigned int   nb_stream_indexes;
    AVDictionary *metadata;

    int program_num;
    int pmt_pid;
    int pcr_pid;
    int pmt_version;

    /*****************************************************************
     * All fields below this line are not part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */
    int64_t start_time;
    int64_t end_time;

    int64_t pts_wrap_reference;    ///< reference dts for wrap detection,换行检测参考dts
    int pts_wrap_behavior;         ///< behavior on wrap detection,换行检测时的行为
} AVProgram;

#define AVFMTCTX_NOHEADER      0x0001 /**< signal that no header is present
                                         (streams are added dynamically)
                                       表示不存在标头（动态添加流）*/
#define AVFMTCTX_UNSEEKABLE    0x0002 /**< signal that the stream is definitely
                                         not seekable, and attempts to call the
                                         seek function will fail. For some
                                         network protocols (e.g. HLS), this can
                                         change dynamically at runtime.
                                      表明该流绝对不可查找，并且尝试调用查找函数将失败。
                                      对于某些网络协议（例如 HLS），这可以在运行时动态更改。*/

typedef struct AVChapter {
    int64_t id;             ///< unique ID to identify the chapter用于识别章节的唯一 ID
    AVRational time_base;   ///< time base in which the start/end timestamps are specified,指定开始/结束时间戳的时基
    int64_t start, end;     ///< chapter start/end time in time_base units,章节开始/结束时间（以 time_base 为单位）
    AVDictionary *metadata;
} AVChapter;


/**
 * Callback used by devices to communicate with application.
 */
typedef int (*av_format_control_message)(struct AVFormatContext *s, int type,
                                         void *data, size_t data_size);

typedef int (*AVOpenCallback)(struct AVFormatContext *s, AVIOContext **pb, const char *url, int flags,
                              const AVIOInterruptCB *int_cb, AVDictionary **options);

/**
 * The duration of a video can be estimated through various ways, and this enum can be used
 * to know how the duration was estimated.
 * 视频的持续时间可以通过多种方式估计，并且可以使用此枚举来了解如何估计持续时间。
 */
enum AVDurationEstimationMethod {
    AVFMT_DURATION_FROM_PTS,    ///< Duration accurately estimated from PTSes,根据 PTS 准确估计的持续时间
    AVFMT_DURATION_FROM_STREAM, ///< Duration estimated from a stream with a known duration,根据已知持续时间的流估计的持续时间
    AVFMT_DURATION_FROM_BITRATE ///< Duration estimated from bitrate (less accurate),根据比特率估计的持续时间（不太准确）
};

/**
 * Format I/O context.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVFormatContext) must not be used outside libav*, use
 * avformat_alloc_context() to create an AVFormatContext.
 *
 * Fields can be accessed through AVOptions (av_opt*),
 * the name string used matches the associated command line parameter name and
 * can be found in libavformat/options_table.h.
 * The AVOption/command line parameter names differ in some cases from the C
 * structure field names for historic reasons or brevity.
 * 格式化I/O上下文。
 * 新字段可以添加到末尾，有较小的版本颠簸。
 * 删除、重新排序和更改现有字段需要主要版本提升。
 * sizeof（AVFormatContext）不能在libav*之外使用，使用avformat_alloc_context（）创建一个AVFormatContext。
 * 字段可以通过AVOptions（av_opt*）访问，使用的名称字符串与关联的命令行参数名称匹配，并且可以在libavform/options_table. h中找到。
 * 由于历史原因或简洁性，AVOption/命令行参数名称在某些情况下与C结构字段名称不同。
 */
typedef struct AVFormatContext {
    /**
     * A class for logging and @ref avoptions. Set by avformat_alloc_context().
     * Exports (de)muxer private options if they exist.
     */
    const AVClass *av_class;

    /**
     * The input container format.输入容器格式。
     *
     * Demuxing only, set by avformat_open_input().仅解复用，由 avformat_open_input() 设置。
     */
    const struct AVInputFormat *iformat;

    /**
     * The output container format.输出容器格式。
     *
     * Muxing only, must be set by the caller before avformat_write_header().
     * 仅复用，必须由调用者在 avformat_write_header() 之前设置。
     */
    const struct AVOutputFormat *oformat;

    /**
     * Format private data. This is an AVOptions-enabled struct
     * if and only if iformat/oformat.priv_class is not NULL.
     *
     * - muxing: set by avformat_write_header()
     * - demuxing: set by avformat_open_input()
     * 格式私有数据。
     * 当且仅当 iformat/oformat.priv_class 不为 NULL 时，这是一个启用 AVOptions 的结构。
     * 混合：由 avformat_write_header() 设置
     * 解复用：由 avformat_open_input() 设置
     */
    void *priv_data;

    /**
     * I/O context.
     *
     * - demuxing: either set by the user before avformat_open_input() (then
     *             the user must close it manually) or set by avformat_open_input().
     * - muxing: set by the user before avformat_write_header(). The caller must
     *           take care of closing / freeing the IO context.
     *
     * Do NOT set this field if AVFMT_NOFILE flag is set in
     * iformat/oformat.flags. In such a case, the (de)muxer will handle
     * I/O in some other way and this field will be NULL.
     * I/O 上下文。
     * 解复用：要么由用户在 avformat_open_input() 之前设置（然后用户必须手动关闭它），
     * 要么由 avformat_open_input() 设置。
     * 复用：由用户在 avformat_write_header() 之前设置。 调用者必须负责关闭/释放 IO 上下文。
     * 如果在 iformat/oformat.flags 中设置了 AVFMT_NOFILE 标志，请勿设置此字段。 在这种情况下，
     * （解）复用器将以其他方式处理 I/O，并且该字段将为 NULL。
     */
    AVIOContext *pb;

    /* stream info */
    /**
     * Flags signalling stream properties. A combination of AVFMTCTX_*.
     * Set by libavformat.
     * 标志信令流属性。
     * AVFMTCTX_* 的组合。 由 libavformat 设置。
     */
    int ctx_flags;

    /**
     * Number of elements in AVFormatContext.streams.
     *
     * Set by avformat_new_stream(), must not be modified by any other code.
     * AVFormatContext.streams 中的元素数量。
     * 由 avformat_new_stream() 设置，不得由任何其他代码修改。
     */
    unsigned int nb_streams;
    /**
     * A list of all streams in the file. New streams are created with
     * avformat_new_stream().
     *
     * - demuxing: streams are created by libavformat in avformat_open_input().
     *             If AVFMTCTX_NOHEADER is set in ctx_flags, then new streams may also
     *             appear in av_read_frame().
     * - muxing: streams are created by the user before avformat_write_header().
     *
     * Freed by libavformat in avformat_free_context().
     * 文件中所有流的列表。
     * 使用 avformat_new_stream() 创建新流。
     * 解复用：流由 libavformat 在 avformat_open_input() 中创建。 如果 ctx_flags 中设置了
     * AVFMTCTX_NOHEADER，那么新的流也可能出现在 av_read_frame() 中。
     * 复用：流由用户在 avformat_write_header() 之前创建。
     * 由 avformat_free_context() 中的 libavformat 释放。
     */
    AVStream **streams;

    /**
     * input or output URL. Unlike the old filename field, this field has no
     * length restriction.
     * 输入或输出 URL。与旧的文件名字段不同，该字段没有长度限制。
     *
     * - demuxing: set by avformat_open_input(), initialized to an empty
     *             string if url parameter was NULL in avformat_open_input().
     *   解复用：由 avformat_open_input() 设置，如果 avformat_open_input() 中 url
     *   参数为 NULL，则初始化为空字符串。
     * - muxing: may be set by the caller before calling avformat_write_header()
     *           (or avformat_init_output() if that is called first) to a string
     *           which is freeable by av_free(). Set to an empty string if it
     *           was NULL in avformat_init_output().
     *   muxing：可以由调用者在调用 avformat_write_header() （或 avformat_init_output()，
     *   如果先调用的话）之前设置为可由 av_free() 释放的字符串。 如果 avformat_init_output()
     *   中为 NULL，则设置为空字符串。
     *
     * Freed by libavformat in avformat_free_context().
     * 由 avformat_free_context() 中的 libavformat 释放。
     */
    char *url;

    /**
     * Position of the first frame of the component, in
     * AV_TIME_BASE fractional seconds. NEVER set this value directly:
     * It is deduced from the AVStream values.
     *
     * Demuxing only, set by libavformat.
     * 组件第一帧的位置，以 AV_TIME_BASE 小数秒为单位。
     * 切勿直接设置该值：它是从 AVStream 值推导出来的。
     * 仅解复用，由 libavformat 设置。
     */
    int64_t start_time;

    /**
     * Duration of the stream, in AV_TIME_BASE fractional
     * seconds. Only set this value if you know none of the individual stream
     * durations and also do not set any of them. This is deduced from the
     * AVStream values if not set.
     *
     * Demuxing only, set by libavformat.
     * 流的持续时间，以 AV_TIME_BASE 小数秒为单位。
     * 仅当您不知道任何单个流持续时间并且也不设置任何一个时才设置此值。 如果未设置，则从 AVStream 值推导出来。
     * 仅解复用，由 libavformat 设置。
     */
    int64_t duration;

    /**
     * Total stream bitrate in bit/s, 0 if not
     * available. Never set it directly if the file_size and the
     * duration are known as FFmpeg can compute it automatically.
     * 总流比特率（以位/秒为单位），如果不可用则为 0。
     * 如果 file_size 和持续时间已知，FFmpeg 可以自动计算，切勿直接设置它。
     */
    int64_t bit_rate;

    unsigned int packet_size;
    int max_delay;

    /**
     * Flags modifying the (de)muxer behaviour. A combination of AVFMT_FLAG_*.
     * Set by the user before avformat_open_input() / avformat_write_header().
     * 修改（解）复用器行为的标志。
     * AVFMT_FLAG_* 的组合。 由用户在 avformat_open_input() / avformat_write_header() 之前设置。
     */
    int flags;
#define AVFMT_FLAG_GENPTS       0x0001 ///< Generate missing pts even if it requires parsing future frames.
                                       ///< 即使需要解析未来的帧，也会生成丢失的点。
#define AVFMT_FLAG_IGNIDX       0x0002 ///< Ignore index.忽略索引。
#define AVFMT_FLAG_NONBLOCK     0x0004 ///< Do not block when reading packets from input.
                                       ///< 从输入读取数据包时不要阻塞。
#define AVFMT_FLAG_IGNDTS       0x0008 ///< Ignore DTS on frames that contain both DTS & PTS
                                       ///< 忽略同时包含 DTS 和 PTS 的帧上的 DTS
#define AVFMT_FLAG_NOFILLIN     0x0010 ///< Do not infer any values from other values, just return what is stored in the container
                                       ///< 不从其他值推断任何值，仅返回容器中存储的内容
#define AVFMT_FLAG_NOPARSE      0x0020 ///< Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
                                       ///< 不要使用 AVParsers，您还必须设置 AVFMT_FLAG_NOFILLIN，因为填充代码适用于帧并且没有解析 -> 无帧。 如果已禁用解析查找帧边界，则查找帧也无法工作
#define AVFMT_FLAG_NOBUFFER     0x0040 ///< Do not buffer frames when possible,尽可能不缓冲帧
#define AVFMT_FLAG_CUSTOM_IO    0x0080 ///< The caller has supplied a custom AVIOContext, don't avio_close() it.
                                       ///< 调用者提供了一个自定义的 AVIOContext，不要对其进行 avio_close() 。
#define AVFMT_FLAG_DISCARD_CORRUPT  0x0100 ///< Discard frames marked corrupted,丢弃标记为损坏的帧
#define AVFMT_FLAG_FLUSH_PACKETS    0x0200 ///< Flush the AVIOContext every packet.每个数据包刷新 AVIOContext。
/**
 * When muxing, try to avoid writing any random/volatile data to the output.
 * This includes any random IDs, real-time timestamps/dates, muxer version, etc.
 * 复用时，尽量避免将任何随机/易失性数据写入输出。
 * 这包括任何随机 ID、实时时间戳/日期、复用器版本等。
 *
 * This flag is mainly intended for testing.该标志主要用于测试。
 */
#define AVFMT_FLAG_BITEXACT         0x0400
#define AVFMT_FLAG_SORT_DTS    0x10000 ///< try to interleave outputted packets by dts (using this flag can slow demuxing down)
                                       ///< 尝试通过 dts 交错输出数据包（使用此标志会减慢解复用速度）
#define AVFMT_FLAG_FAST_SEEK   0x80000 ///< Enable fast, but inaccurate seeks for some formats对某些格式启用快速但不准确的搜索
#define AVFMT_FLAG_SHORTEST   0x100000 ///< Stop muxing when the shortest stream stops.当最短流停止时停止复用。
#define AVFMT_FLAG_AUTO_BSF   0x200000 ///< Add bitstream filters as requested by the muxer根据复用器的要求添加比特流过滤器

    /**
     * Maximum number of bytes read from input in order to determine stream
     * properties. Used when reading the global header and in
     * avformat_find_stream_info().
     * 为了确定流属性而从输入读取的最大字节数。
     * 在读取全局标头和 avformat_find_stream_info() 时使用。
     *
     * Demuxing only, set by the caller before avformat_open_input().
     * 仅解复用，由调用者在 avformat_open_input() 之前设置。
     *
     * @note this is \e not  used for determining the \ref AVInputFormat
     *       "input format"
     * 这不用于确定输入格式
     * @sa format_probesize
     */
    int64_t probesize;

    /**
     * Maximum duration (in AV_TIME_BASE units) of the data read
     * from input in avformat_find_stream_info().
     * Demuxing only, set by the caller before avformat_find_stream_info().
     * Can be set to 0 to let avformat choose using a heuristic.
     * 从 avformat_find_stream_info() 中的输入读取的数据的最大持续时间（以 AV_TIME_BASE 为单位）。
     * 仅解复用，由调用者在 avformat_find_stream_info() 之前设置。 可以设置为 0 让 avformat 使用启发式进行选择
     */
    int64_t max_analyze_duration;

    const uint8_t *key;
    int keylen;

    unsigned int nb_programs;
    AVProgram **programs;

    /**
     * Forced video codec_id.
     * Demuxing: Set by user.
     */
    enum AVCodecID video_codec_id;

    /**
     * Forced audio codec_id.
     * Demuxing: Set by user.
     */
    enum AVCodecID audio_codec_id;

    /**
     * Forced subtitle codec_id.
     * Demuxing: Set by user.
     */
    enum AVCodecID subtitle_codec_id;

    /**
     * Maximum amount of memory in bytes to use for the index of each stream.
     * If the index exceeds this size, entries will be discarded as
     * needed to maintain a smaller size. This can lead to slower or less
     * accurate seeking (depends on demuxer).
     * Demuxers for which a full in-memory index is mandatory will ignore
     * this.
     * 用于每个流索引的最大内存量（以字节为单位）。
     * 如果索引超过此大小，则将根据需要丢弃条目以保持较小的大小。
     * 这可能会导致查找速度变慢或不太准确（取决于解复用器）。 强制使用完整内存索引的解复用器将忽略这一点。
     * - muxing: unused
     * - demuxing: set by user
     * 多路复用：未使用
     * 解复用：由用户设置
     */
    unsigned int max_index_size;

    /**
     * Maximum amount of memory in bytes to use for buffering frames
     * obtained from realtime capture devices.
     * 用于缓冲从实时捕获设备获取的帧的最大内存量（以字节为单位）。
     */
    unsigned int max_picture_buffer;

    /**
     * Number of chapters in AVChapter array.
     * When muxing, chapters are normally written in the file header,
     * so nb_chapters should normally be initialized before write_header
     * is called. Some muxers (e.g. mov and mkv) can also write chapters
     * in the trailer.  To write chapters in the trailer, nb_chapters
     * must be zero when write_header is called and non-zero when
     * write_trailer is called.
     * - muxing: set by user
     * - demuxing: set by libavformat
     * AVChatter 数组中的章节数。
     * 复用时，章节通常写入文件头中，因此 nb_chapters 通常应在调用 write_header 之前初始化。
     * 一些混合器（例如 mov 和 mkv）也可以在结尾中编写章节。 要在结尾中写入章节，调用 write_header 时
     * nb_chapters 必须为零，调用 write_trailer 时 nb_chapters 必须非零。
     * 多路复用：由用户设置
     * 解复用：由 libavformat 设置
     */
    unsigned int nb_chapters;
    AVChapter **chapters;

    /**
     * Metadata that applies to the whole file.
     *
     * - demuxing: set by libavformat in avformat_open_input()
     * - muxing: may be set by the caller before avformat_write_header()
     *
     * Freed by libavformat in avformat_free_context().
     * 适用于整个文件的元数据。
     * 解复用：由 avformat_open_input() 中的 libavformat 设置
     * 复用：可以由调用者在 avformat_write_header() 之前设置
     * 由 avformat_free_context() 中的 libavformat 释放。
     */
    AVDictionary *metadata;

    /**
     * Start time of the stream in real world time, in microseconds
     * since the Unix epoch (00:00 1st January 1970). That is, pts=0 in the
     * stream was captured at this real world time.
     * - muxing: Set by the caller before avformat_write_header(). If set to
     *           either 0 or AV_NOPTS_VALUE, then the current wall-time will
     *           be used.
     * - demuxing: Set by libavformat. AV_NOPTS_VALUE if unknown. Note that
     *             the value may become known after some number of frames
     *             have been received.
     * 现实世界时间中流的开始时间，以 Unix 纪元（1970 年 1 月 1 日 00:00）以来的微秒为单位。
     * 也就是说，流中的 pts=0 是在这个真实世界时间捕获的。
     * 复用：由调用者在 avformat_write_header() 之前设置。 如果设置为 0 或 AV_NOPTS_VALUE，则将使用当前的挂墙时间。
     * 解复用：由 libavformat 设置。 AV_NOPTS_VALUE（如果未知）。 请注意，在接收到一定数量的帧后，该值可能会变为已知。
     */
    int64_t start_time_realtime;

    /**
     * The number of frames used for determining the framerate in
     * avformat_find_stream_info().
     * Demuxing only, set by the caller before avformat_find_stream_info().
     * 用于确定 avformat_find_stream_info() 中帧速率的帧数。
     * 仅解复用，由调用者在 avformat_find_stream_info() 之前设置。
     */
    int fps_probe_size;

    /**
     * Error recognition; higher values will detect more errors but may
     * misdetect some more or less valid parts as errors.
     * Demuxing only, set by the caller before avformat_open_input().
     * 错误识别； 较高的值将检测到更多错误，但可能会将一些或多或少有效的部分误检测为错误。
     * 仅解复用，由调用者在 avformat_open_input() 之前设置。
     */
    int error_recognition;

    /**
     * Custom interrupt callbacks for the I/O layer.
     *
     * demuxing: set by the user before avformat_open_input().
     * muxing: set by the user before avformat_write_header()
     * (mainly useful for AVFMT_NOFILE formats). The callback
     * should also be passed to avio_open2() if it's used to
     * open the file.
     * I/O 层的自定义中断回调。
     * 解复用：由用户在 avformat_open_input() 之前设置。
     * 复用：由用户在 avformat_write_header() 之前设置（主要对 AVFMT_NOFILE 格式有用）。
     * 如果用于打开文件，回调也应该传递给 avio_open2()。
     */
    AVIOInterruptCB interrupt_callback;

    /**
     * Flags to enable debugging.
     */
    int debug;
#define FF_FDEBUG_TS        0x0001

    /**
     * Maximum buffering duration for interleaving.
     * 交错的最大缓冲持续时间。
     *
     * To ensure all the streams are interleaved correctly,
     * av_interleaved_write_frame() will wait until it has at least one packet
     * for each stream before actually writing any packets to the output file.
     * When some streams are "sparse" (i.e. there are large gaps between
     * successive packets), this can result in excessive buffering.
     * 为了确保所有流正确交错，av_interleaved_write_frame() 将等待，直到每个流至少有
     * 一个数据包，然后才将任何数据包实际写入输出文件。 当某些流“稀疏”时（即连续数据包
     * 之间存在较大间隙），这可能会导致过度缓冲。
     *
     * This field specifies the maximum difference between the timestamps of the
     * first and the last packet in the muxing queue, above which libavformat
     * will output a packet regardless of whether it has queued a packet for all
     * the streams.
     * 该字段指定多路复用队列中第一个数据包和最后一个数据包的时间戳之间的最大差异，
     * 高于该值的 libavformat 将输出一个数据包，无论它是否已将所有流的数据包排队。
     *
     * Muxing only, set by the caller before avformat_write_header().
     * 仅复用，由调用者在 avformat_write_header() 之前设置。
     */
    int64_t max_interleave_delta;

    /**
     * Allow non-standard and experimental extension
     * 允许非标准和实验性扩展。
     * @see AVCodecContext.strict_std_compliance
     */
    int strict_std_compliance;

    /**
     * Flags indicating events happening on the file, a combination of
     * AVFMT_EVENT_FLAG_*.
     * 指示文件上发生的事件的标志，AVFMT_EVENT_FLAG_* 的组合。
     *
     * - demuxing: may be set by the demuxer in avformat_open_input(),
     *   avformat_find_stream_info() and av_read_frame(). Flags must be cleared
     *   by the user once the event has been handled.
     *   解复用：可以由解复用器在 avformat_open_input()、avformat_find_stream_info()
     *   和 av_read_frame() 中设置。 处理事件后，用户必须清除标志。
     * - muxing: may be set by the user after avformat_write_header() to
     *   indicate a user-triggered event.  The muxer will clear the flags for
     *   events it has handled in av_[interleaved]_write_frame().
     *   muxing：可以由用户在 avformat_write_header() 之后设置，以指示用户触发的事件。
     *   复用器将清除它在 av_[interleaved]_write_frame() 中处理的事件的标志。
     *
     */
    int event_flags;
/**
 * - demuxing: the demuxer read new metadata from the file and updated
 *   AVFormatContext.metadata accordingly
 *   解复用：解复用器从文件中读取新的元数据并相应地更新 AVFormatContext.metadata
 * - muxing: the user updated AVFormatContext.metadata and wishes the muxer to
 *   write it into the file
 *   复用：用户更新了 AVFormatContext.metadata 并希望 muxer 将其写入文件
 */
#define AVFMT_EVENT_FLAG_METADATA_UPDATED 0x0001

    /**
     * Maximum number of packets to read while waiting for the first timestamp.
     * Decoding only.
     * 等待第一个时间戳时读取的最大数据包数。
     * 仅解码。
     */
    int max_ts_probe;

    /**
     * Avoid negative timestamps during muxing.
     * Any value of the AVFMT_AVOID_NEG_TS_* constants.
     * Note, this works better when using av_interleaved_write_frame().
     * - muxing: Set by user
     * - demuxing: unused
     * 在复用期间避免负时间戳。AVFMT_AVOID_NEG_TS_* 常量的任何值。
     * 请注意，使用 av_interleaved_write_frame() 时效果更好。
     * - 多路复用：由用户设置
     * - 解复用：未使用
     */
    int avoid_negative_ts;
#define AVFMT_AVOID_NEG_TS_AUTO             -1 ///< Enabled when required by target format，当目标格式需要时启用
#define AVFMT_AVOID_NEG_TS_DISABLED          0 ///< Do not shift timestamps even when they are negative.即使时间戳为负数，也不要移动时间戳。
#define AVFMT_AVOID_NEG_TS_MAKE_NON_NEGATIVE 1 ///< Shift timestamps so they are non negative，移动时间戳，使其成为非负数
#define AVFMT_AVOID_NEG_TS_MAKE_ZERO         2 ///< Shift timestamps so that they start at 0，移动时间戳，使其从 0 开始

    /**
     * Transport stream id.
     * This will be moved into demuxer private options. Thus no API/ABI compatibility
     * 传输流 ID。
     * 这将被移至解复用器私有选项中。 因此没有 API/ABI 兼容性
     */
    int ts_id;

    /**
     * Audio preload in microseconds.
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     * 音频预加载（以微秒为单位）。
     * 请注意，并非所有格式都支持此功能，如果在不支持的情况下使用它，可能会发生不可预测的事情。
     * 编码：由用户设置
     * 解码：未使用
     */
    int audio_preload;

    /**
     * Max chunk time in microseconds.
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     * 最大块时间（以微秒为单位）。
     * 请注意，并非所有格式都支持此功能，如果在不支持的情况下使用它，可能会发生不可预测的事情。
     * 编码：由用户设置
     * 解码：未使用
     */
    int max_chunk_duration;

    /**
     * Max chunk size in bytes
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     * 最大块大小（以字节为单位）请注意，并非所有格式都支持此功能，如果在不支持的情况下使用它，可能会发生不可预测的事情。
     * 编码：由用户设置
     * 解码：未使用
     */
    int max_chunk_size;

    /**
     * forces the use of wallclock timestamps as pts/dts of packets
     * This has undefined results in the presence of B frames.
     * - encoding: unused
     * - decoding: Set by user
     * 强制使用挂钟时间戳作为数据包的 pts/dts 这在存在 B 帧的情况下会产生未定义的结果。
     * 编码：未使用
     * 解码：由用户设置
     */
    int use_wallclock_as_timestamps;

    /**
     * avio flags, used to force AVIO_FLAG_DIRECT.
     * avio 标志，用于强制 AVIO_FLAG_DIRECT。
     * - encoding: unused
     * - decoding: Set by user
     */
    int avio_flags;

    /**
     * The duration field can be estimated through various ways, and this field can be used
     * to know how the duration was estimated.
     * - encoding: unused
     * - decoding: Read by user
     * 可以通过多种方式估计持续时间字段，并且可以使用该字段来了解持续时间是如何估计的。
     * 编码：未使用
     * 解码：由用户读取
     */
    enum AVDurationEstimationMethod duration_estimation_method;

    /**
     * Skip initial bytes when opening stream
     * - encoding: unused
     * - decoding: Set by user
     * 打开流时跳过初始字节。
     * 编码：未使用
     * 解码：由用户设置
     */
    int64_t skip_initial_bytes;

    /**
     * Correct single timestamp overflows
     * - encoding: unused
     * - decoding: Set by user
     * 纠正单个时间戳溢出。
     * 编码：未使用
     * 解码：由用户设置
     */
    unsigned int correct_ts_overflow;

    /**
     * Force seeking to any (also non key) frames.
     * - encoding: unused
     * - decoding: Set by user
     * 强制寻找任何（也非关键）帧。
     * 编码：未使用
     * 解码：由用户设置
     */
    int seek2any;

    /**
     * Flush the I/O context after each packet.
     * - encoding: Set by user
     * - decoding: unused
     * 每个数据包后刷新 I/O 上下文。
     * 编码：由用户设置
     * 解码：未使用
     */
    int flush_packets;

    /**
     * format probing score.
     * The maximal score is AVPROBE_SCORE_MAX, its set when the demuxer probes
     * the format.
     * - encoding: unused
     * - decoding: set by avformat, read by user
     * 格式化探测分数。
     * 最大分数是 AVPROBE_SCORE_MAX，它是在解复用器探测格式时设置的。
     * 编码：未使用
     * 解码：由avformat设置，由用户读取
     */
    int probe_score;

    /**
     * Maximum number of bytes read from input in order to identify the
     * \ref AVInputFormat "input format". Only used when the format is not set
     * explicitly by the caller.
     *
     * Demuxing only, set by the caller before avformat_open_input().
     *
     * @sa probesize
     * 为了识别输入格式而从输入读取的最大字节数。
     * 仅当调用者未明确设置格式时才使用。
     * 仅解复用，由调用者在 avformat_open_input() 之前设置。
     */
    int format_probesize;

    /**
     * ',' separated list of allowed decoders.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     * ',' 分隔的允许解码器列表。
     * 如果为 NULL，则所有内容均被允许
     * 编码：未使用
     * 解码：由用户设置
     */
    char *codec_whitelist;

    /**
     * ',' separated list of allowed demuxers.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     * ',' 分隔的允许解复用器列表。
     * 如果为 NULL，则所有内容均被允许
     * 编码：未使用
     * 解码：由用户设置
     */
    char *format_whitelist;

    /**
     * IO repositioned flag.
     * This is set by avformat when the underlaying IO context read pointer
     * is repositioned, for example when doing byte based seeking.
     * Demuxers can use the flag to detect such changes.
     * IO 重新定位标志。当底层 IO 上下文读指针被重新定位时，例如在进行基于字节的查找时，
     * 这是由 avformat 设置的。 解复用器可以使用该标志来检测此类更改。
     */
    int io_repositioned;

    /**
     * Forced video codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     * 强制视频编解码器。
     * 这允许强制使用特定的解码器，即使存在多个具有相同 codec_id 的解码器。
     * 解复用：由用户设置
     */
    const AVCodec *video_codec;

    /**
     * Forced audio codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     * 强制音频编解码器。
     * 这允许强制使用特定的解码器，即使存在多个具有相同 codec_id 的解码器。
     * 解复用：由用户设置
     */
    const AVCodec *audio_codec;

    /**
     * Forced subtitle codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     * 强制字幕编解码器。
     * 这允许强制使用特定的解码器，即使存在多个具有相同 codec_id 的解码器。
     * 解复用：由用户设置
     */
    const AVCodec *subtitle_codec;

    /**
     * Forced data codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     * 强制数据编解码器。
     * 这允许强制使用特定的解码器，即使存在多个具有相同 codec_id 的解码器。
     * 解复用：由用户设置
     */
    const AVCodec *data_codec;

    /**
     * Number of bytes to be written as padding in a metadata header.
     * Demuxing: Unused.
     * Muxing: Set by user via av_format_set_metadata_header_padding.
     * 要写入元数据标头中填充的字节数。
     * 解复用：未使用。
     * 复用：由用户通过 av_format_set_metadata_header_padding 设置。
     */
    int metadata_header_padding;

    /**
     * User data.用户数据。
     * This is a place for some private data of the user.
     * 这是用户的一些私人数据的地方。
     */
    void *opaque;

    /**
     * Callback used by devices to communicate with application.
     * 设备使用回调与应用程序进行通信。
     */
    av_format_control_message control_message_cb;

    /**
     * Output timestamp offset, in microseconds.
     * Muxing: set by user
     * 输出时间戳偏移量，以微秒为单位。
     * 复用：由用户设置
     */
    int64_t output_ts_offset;

    /**
     * dump format separator.
     * can be ", " or "\n      " or anything else
     * - muxing: Set by user.
     * - demuxing: Set by user.
     * 转储格式分隔符。
     * 可以是“,”或“\n”或其他任何内容
     * 多路复用：由用户设置。
     * 解复用：由用户设置。
     */
    uint8_t *dump_separator;

    /**
     * Forced Data codec_id.
     * Demuxing: Set by user.
     */
    enum AVCodecID data_codec_id;

    /**
     * ',' separated list of allowed protocols.
     * - encoding: unused
     * - decoding: set by user
     * ',' 分隔的允许协议列表。
     * 编码：未使用
     * 解码：由用户设置
     */
    char *protocol_whitelist;

    /**
     * A callback for opening new IO streams.
     * 打开新 IO 流的回调。
     *
     * Whenever a muxer or a demuxer needs to open an IO stream (typically from
     * avformat_open_input() for demuxers, but for certain formats can happen at
     * other times as well), it will call this callback to obtain an IO context.
     * 每当复用器或解复用器需要打开 IO 流（通常来自解复用器的 avformat_open_input() ，
     * 但对于某些格式也可能在其他时间发生），它将调用此回调来获取 IO 上下文。
     *
     * @param s the format context,格式上下文
     * @param pb on success, the newly opened IO context should be returned here
     * 成功后，应在此处返回新打开的 IO 上下文
     * @param url the url to open要打开的网址
     * @param flags a combination of AVIO_FLAG_*
     * @param options a dictionary of additional options, with the same
     *                semantics as in avio_open2()
     * @return 0 on success, a negative AVERROR code on failure
     *
     * @note Certain muxers and demuxers do nesting, i.e. they open one or more
     * additional internal format contexts. Thus the AVFormatContext pointer
     * passed to this callback may be different from the one facing the caller.
     * It will, however, have the same 'opaque' field.
     * 某些复用器和解复用器会进行嵌套，即它们打开一个或多个附加的内部格式上下文。 因此，
     * 传递给此回调的 AVFormatContext 指针可能与面向调用者的指针不同。 然而，它将具有相同的“不透明”字段。
     */
    int (*io_open)(struct AVFormatContext *s, AVIOContext **pb, const char *url,
                   int flags, AVDictionary **options);

#if FF_API_AVFORMAT_IO_CLOSE
    /**
     * A callback for closing the streams opened with AVFormatContext.io_open().
     * 用于关闭使用 AVFormatContext.io_open() 打开的流的回调。
     *
     * @deprecated use io_close2
     */
    attribute_deprecated
    void (*io_close)(struct AVFormatContext *s, AVIOContext *pb);
#endif

    /**
     * ',' separated list of disallowed protocols.
     * - encoding: unused
     * - decoding: set by user
     * ',' 分隔的不允许的协议列表。
     * 编码：未使用
     * 解码：由用户设置
     */
    char *protocol_blacklist;

    /**
     * The maximum number of streams.
     * - encoding: unused
     * - decoding: set by user
     * 最大流数。
     * 编码：未使用
     * 解码：由用户设置
     */
    int max_streams;

    /**
     * Skip duration calcuation in estimate_timings_from_pts.
     * - encoding: unused
     * - decoding: set by user
     * 跳过estimate_timings_from_pts中的持续时间计算。
     * 编码：未使用
     * 解码：由用户设置
     */
    int skip_estimate_duration_from_pts;

    /**
     * Maximum number of packets that can be probed
     * - encoding: unused
     * - decoding: set by user
     * 可以探测的最大数据包数。
     * 编码：未使用
     * 解码：由用户设置
     */
    int max_probe_packets;

    /**
     * A callback for closing the streams opened with AVFormatContext.io_open().
     * 用于关闭使用 AVFormatContext.io_open() 打开的流的回调。
     *
     * Using this is preferred over io_close, because this can return an error.
     * Therefore this callback is used instead of io_close by the generic
     * libavformat code if io_close is NULL or the default.
     * 使用它优于 io_close，因为这可能会返回错误。 因此，如果 io_close 为 NULL 或默认值，
     * 通用 libavformat 代码将使用此回调代替 io_close。
     *
     * @param s the format context
     * @param pb IO context to be closed and freed
     * @return 0 on success, a negative AVERROR code on failure
     */
    int (*io_close2)(struct AVFormatContext *s, AVIOContext *pb);
} AVFormatContext;

/**
 * This function will cause global side data to be injected in the next packet
 * of each stream as well as after any subsequent seek.
 * 此函数将导致全局侧数据被注入到每个流的下一个数据包中以及在任何后续查找之后。
 */
void av_format_inject_global_side_data(AVFormatContext *s);

/**
 * Returns the method used to set ctx->duration.
 *
 * @return AVFMT_DURATION_FROM_PTS, AVFMT_DURATION_FROM_STREAM, or AVFMT_DURATION_FROM_BITRATE.
 */
enum AVDurationEstimationMethod av_fmt_ctx_get_duration_estimation_method(const AVFormatContext* ctx);

/**
 * @defgroup lavf_core Core functions
 * @ingroup libavf
 *
 * Functions for querying libavformat capabilities, allocating core structures,
 * etc.
 * 查询libavformat能力、分配核心结构等功能。
 * @{
 */

/**
 * Return the LIBAVFORMAT_VERSION_INT constant.
 */
unsigned avformat_version(void);

/**
 * Return the libavformat build-time configuration.
 */
const char *avformat_configuration(void);

/**
 * Return the libavformat license.
 */
const char *avformat_license(void);

/**
 * Do global initialization of network libraries. This is optional,
 * and not recommended anymore.
 * 对网络库进行全局初始化。这是可选的，不再推荐。
 *
 * This functions only exists to work around thread-safety issues
 * with older GnuTLS or OpenSSL libraries. If libavformat is linked
 * to newer versions of those libraries, or if you do not use them,
 * calling this function is unnecessary. Otherwise, you need to call
 * this function before any other threads using them are started.
 * 此函数的存在只是为了解决旧版 GnuTLS 或 OpenSSL 库的线程安全问题。
 * 如果 libavformat 链接到这些库的较新版本，或者如果您不使用它们，则无需调用此函数。
 * 否则，您需要在使用它们的任何其他线程启动之前调用此函数。
 *
 * This function will be deprecated once support for older GnuTLS and
 * OpenSSL libraries is removed, and this function has no purpose
 * anymore.
 * 一旦删除对旧版 GnuTLS 和 OpenSSL 库的支持，此函数将被弃用，并且此函数不再有任何用途。
 */
int avformat_network_init(void);

/**
 * Undo the initialization done by avformat_network_init. Call it only
 * once for each time you called avformat_network_init.
 * 撤消 avformat_network_init 完成的初始化。每次调用 avformat_network_init 时仅调用一次。
 */
int avformat_network_deinit(void);

/**
 * Iterate over all registered muxers.
 * 迭代所有已注册的复用器。
 *
 * @param opaque a pointer where libavformat will store the iteration state. Must
 *               point to NULL to start the iteration.
 * 不透明的指针，libavformat 将在其中存储迭代状态。 必须指向 NULL 才能开始迭代。
 *
 * @return the next registered muxer or NULL when the iteration is
 *         finished
 */
const AVOutputFormat *av_muxer_iterate(void **opaque);

/**
 * Iterate over all registered demuxers.
 * 迭代所有已注册的解复用器。
 *
 * @param opaque a pointer where libavformat will store the iteration state.
 *               Must point to NULL to start the iteration.
 * 不透明的指针，libavformat 将在其中存储迭代状态。 必须指向 NULL 才能开始迭代。
 *
 * @return the next registered demuxer or NULL when the iteration is
 *         finished
 */
const AVInputFormat *av_demuxer_iterate(void **opaque);

/**
 * Allocate an AVFormatContext.分配一个 AVFormatContext。
 * avformat_free_context() can be used to free the context and everything
 * allocated by the framework within it.
 * avformat_free_context() 可用于释放上下文以及框架在其中分配的所有内容。
 */
AVFormatContext *avformat_alloc_context(void);

/**
 * Free an AVFormatContext and all its streams.
 * 释放 AVFormatContext 及其所有流。
 * @param s context to free
 */
void avformat_free_context(AVFormatContext *s);

/**
 * Get the AVClass for AVFormatContext. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 * 获取 AVFormatContext 的 AVClass。
 * 它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用来检查选项。
 *
 * @see av_opt_find().
 */
const AVClass *avformat_get_class(void);

/**
 * Get the AVClass for AVStream. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 * 获取 AVStream 的 AVClass。
 * 它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用来检查选项。
 *
 * @see av_opt_find().
 */
const AVClass *av_stream_get_class(void);

/**
 * Add a new stream to a media file.
 * 将新流添加到媒体文件。
 *
 * When demuxing, it is called by the demuxer in read_header(). If the
 * flag AVFMTCTX_NOHEADER is set in s.ctx_flags, then it may also
 * be called in read_packet().
 * 解复用时，由解复用器在read_header()中调用。 如果在 s.ctx_flags 中设置了标志
 * AVFMTCTX_NOHEADER，那么也可以在 read_packet() 中调用它。
 *
 * When muxing, should be called by the user before avformat_write_header().
 * 复用时，用户应在 avformat_write_header() 之前调用。
 *
 * User is required to call avformat_free_context() to clean up the allocation
 * by avformat_new_stream().
 * 用户需要调用 avformat_free_context() 来清理 avformat_new_stream() 的分配。
 *
 * @param s media file handle
 * @param c unused, does nothing
 *
 * @return newly created stream or NULL on error.
 */
AVStream *avformat_new_stream(AVFormatContext *s, const AVCodec *c);

/**
 * Wrap an existing array as stream side data.
 * 将现有数组包装为流端辅助数据。
 *
 * @param st   stream
 * @param type side information type
 * @param data the side data array. It must be allocated with the av_malloc()
 *             family of functions. The ownership of the data is transferred to
 *             st.
 * @param size side information size
 *
 * @return zero on success, a negative AVERROR code on failure. On failure,
 *         the stream is unchanged and the data remains owned by the caller.
 */
int av_stream_add_side_data(AVStream *st, enum AVPacketSideDataType type,
                            uint8_t *data, size_t size);

/**
 * Allocate new information from stream.
 * 从流中分配新信息。
 *
 * @param stream stream
 * @param type   desired side information type
 * @param size   side information size
 *
 * @return pointer to fresh allocated data or NULL otherwise
 */
uint8_t *av_stream_new_side_data(AVStream *stream,
                                 enum AVPacketSideDataType type, size_t size);
/**
 * Get side information from stream.
 * 从流中获取辅助信息。
 *
 * @param stream stream
 * @param type   desired side information type
 * @param size   If supplied, *size will be set to the size of the side data
 *               or to zero if the desired side data is not present.
 *
 * @return pointer to data if present or NULL otherwise
 */
uint8_t *av_stream_get_side_data(const AVStream *stream,
                                 enum AVPacketSideDataType type, size_t *size);

AVProgram *av_new_program(AVFormatContext *s, int id);

/**
 * @}
 */


/**
 * Allocate an AVFormatContext for an output format.
 * 为输出格式分配 AVFormatContext。
 * avformat_free_context() can be used to free the context and
 * everything allocated by the framework within it.
 * avformat_free_context() 可用于释放上下文以及框架在其中分配的所有内容。
 *
 * @param ctx           pointee is set to the created format context,
 *                      or to NULL in case of failure
 *                      pointee 设置为创建的格式上下文，或者在失败时设置为 NULL
 * @param oformat       format to use for allocating the context, if NULL
 *                      format_name and filename are used instead
 *                      用于分配上下文的格式，如果为 NULL，则使用 format_name 和 filename
 * @param format_name   the name of output format to use for allocating the
 *                      context, if NULL filename is used instead
 *                      用于分配上下文的输出格式的名称，如果为 NULL，则使用filename
 * @param filename      the name of the filename to use for allocating the
 *                      context, may be NULL
 *                      用于分配上下文的文件名，可以为 NULL
 *
 * @return  >= 0 in case of success, a negative AVERROR code in case of
 *          failure
 */
int avformat_alloc_output_context2(AVFormatContext **ctx, const AVOutputFormat *oformat,
                                   const char *format_name, const char *filename);

/**
 * @addtogroup lavf_decoding
 * @{
 */

/**
 * Find AVInputFormat based on the short name of the input format.
 * 根据输入格式的简称找到AVInputFormat。
 */
const AVInputFormat *av_find_input_format(const char *short_name);

/**
 * Guess the file format.猜猜文件格式。
 *
 * @param pd        data to be probed
 * @param is_opened Whether the file is already opened; determines whether
 *                  demuxers with or without AVFMT_NOFILE are probed.
 * 文件是否已经打开； 确定是否探测带有或不带有 AVFMT_NOFILE 的解复用器。
 */
const AVInputFormat *av_probe_input_format(const AVProbeData *pd, int is_opened);

/**
 * Guess the file format.猜猜文件格式。
 *
 * @param pd        data to be probed，待探查的数据
 * @param is_opened Whether the file is already opened; determines whether
 *                  demuxers with or without AVFMT_NOFILE are probed.
 * 文件是否已经打开； 确定是否探测带有或不带有 AVFMT_NOFILE 的解复用器。
 * @param score_max A probe score larger that this is required to accept a
 *                  detection, the variable is set to the actual detection
 *                  score afterwards.
 *                  If the score is <= AVPROBE_SCORE_MAX / 4 it is recommended
 *                  to retry with a larger probe buffer.
 * 探测分数大于接受检测所需的分数，之后将该变量设置为实际检测分数。 如果
 * 分数 <= AVPROBE_SCORE_MAX / 4，建议使用更大的探测缓冲区重试。
 */
const AVInputFormat *av_probe_input_format2(const AVProbeData *pd,
                                            int is_opened, int *score_max);

/**
 * Guess the file format.猜猜文件格式。
 *
 * @param is_opened Whether the file is already opened; determines whether
 *                  demuxers with or without AVFMT_NOFILE are probed.
 * 文件是否已经打开； 确定是否探测带有或不带有 AVFMT_NOFILE 的解复用器。
 * @param score_ret The score of the best detection.
 * 最佳检测的得分。
 */
const AVInputFormat *av_probe_input_format3(const AVProbeData *pd,
                                            int is_opened, int *score_ret);

/**
 * Probe a bytestream to determine the input format. Each time a probe returns
 * with a score that is too low, the probe buffer size is increased and another
 * attempt is made. When the maximum probe size is reached, the input format
 * with the highest score is returned.
 * 探测字节流以确定输入格式。每次探测返回的分数太低时，都会增加探测缓冲区大小并进行另一次尝试。
 * 当达到最大探测大小时，返回得分最高的输入格式。
 *
 * @param pb             the bytestream to probe，要探测的字节流
 * @param fmt            the input format is put here，输入格式放在这里
 * @param url            the url of the stream
 * @param logctx         the log context
 * @param offset         the offset within the bytestream to probe from
 * @param max_probe_size the maximum probe buffer size (zero for default)
 *
 * @return the score in case of success, a negative value corresponding to an
 *         the maximal score is AVPROBE_SCORE_MAX
 *         AVERROR code otherwise
 */
int av_probe_input_buffer2(AVIOContext *pb, const AVInputFormat **fmt,
                           const char *url, void *logctx,
                           unsigned int offset, unsigned int max_probe_size);

/**
 * Like av_probe_input_buffer2() but returns 0 on success
 */
int av_probe_input_buffer(AVIOContext *pb, const AVInputFormat **fmt,
                          const char *url, void *logctx,
                          unsigned int offset, unsigned int max_probe_size);

/**
 * Open an input stream and read the header. The codecs are not opened.
 * The stream must be closed with avformat_close_input().
 * 打开输入流并读取标头。 编解码器未打开。 必须使用 avformat_close_input() 关闭流。
 *
 * @param ps       Pointer to user-supplied AVFormatContext (allocated by
 *                 avformat_alloc_context). May be a pointer to NULL, in
 *                 which case an AVFormatContext is allocated by this
 *                 function and written into ps.
 *                 Note that a user-supplied AVFormatContext will be freed
 *                 on failure.
 * 指向用户提供的 AVFormatContext 的指针（由 avformat_alloc_context 分配）。
 * 可能是指向 NULL 的指针，在这种情况下，该函数会分配 AVFormatContext 并将其写入 ps 中。
 * 请注意，用户提供的 AVFormatContext 将在失败时被释放。
 * @param url      URL of the stream to open.
 * @param fmt      If non-NULL, this parameter forces a specific input format.
 *                 Otherwise the format is autodetected.
 * 如果非 NULL，则此参数强制使用特定的输入格式。 否则会自动检测格式。
 * @param options  A dictionary filled with AVFormatContext and demuxer-private
 *                 options.
 *                 On return this parameter will be destroyed and replaced with
 *                 a dict containing options that were not found. May be NULL.
 * 充满 AVFormatContext 和解复用器私有选项的字典。 返回时，此参数将被销毁并替换为包含
 * 未找到的选项的字典。 可能为 NULL。
 *
 * @return 0 on success, a negative AVERROR on failure.
 *
 * @note If you want to use custom IO, preallocate the format context and set its pb field.
 * 如果要使用自定义 IO，请预先分配格式上下文并设置其 pb 字段。
 */
int avformat_open_input(AVFormatContext **ps, const char *url,
                        const AVInputFormat *fmt, AVDictionary **options);

/**
 * Read packets of a media file to get stream information. This
 * is useful for file formats with no headers such as MPEG. This
 * function also computes the real framerate in case of MPEG-2 repeat
 * frame mode.
 * 读取媒体文件的数据包以获取流信息。这对于没有标头的文件格式（例如 MPEG）非常有用。
 * 该函数还计算 MPEG-2 重复帧模式下的实际帧速率。
 * The logical file position is not changed by this function;
 * examined packets may be buffered for later processing.
 * 该函数不会改变逻辑文件位置； 检查过的数据包可能会被缓冲以供以后处理。
 *
 * @param ic media file handle媒体文件句柄
 * @param options  If non-NULL, an ic.nb_streams long array of pointers to
 *                 dictionaries, where i-th member contains options for
 *                 codec corresponding to i-th stream.
 *                 On return each dictionary will be filled with options that were not found.
 * 如果非 NULL，则为 ic.nb_streams 指向字典的指针长数组，其中第 i 个成员包含与第 i 个流对应的编解码器选项。
 * 返回时，每个字典将填充未找到的选项。
 * @return >=0 if OK, AVERROR_xxx on error
 *
 * @note this function isn't guaranteed to open all the codecs, so
 *       options being non-empty at return is a perfectly normal behavior.
 * 此函数不保证打开所有编解码器，因此返回时选项非空是完全正常的行为。
 *
 * @todo Let the user decide somehow what information is needed so that
 *       we do not waste time getting stuff the user does not need.
 * 让用户以某种方式决定需要什么信息，这样我们就不会浪费时间获取用户不需要的信息。
 */
int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options);

/**
 * Find the programs which belong to a given stream.
 * 查找属于给定流的点是节目。
 *
 * @param ic    media file handle媒体文件句柄
 * @param last  the last found program, the search will start after this
 *              program, or from the beginning if it is NULL
 * 最后找到的程序，搜索将从该程序之后开始，如果为 NULL，则从头开始搜索
 * @param s     stream index流索引
 *
 * @return the next program which belongs to s, NULL if no program is found or
 *         the last program is not among the programs of ic.
 * 属于 s 的下一个程序，如果没有找到程序或最后一个程序不在 ic 的程序中，则为 NULL。
 */
AVProgram *av_find_program_from_stream(AVFormatContext *ic, AVProgram *last, int s);

void av_program_add_stream_index(AVFormatContext *ac, int progid, unsigned int idx);

/**
 * Find the "best" stream in the file.
 * 找到文件中的“最佳”流。
 * The best stream is determined according to various heuristics as the most
 * likely to be what the user expects.
 * If the decoder parameter is non-NULL, av_find_best_stream will find the
 * default decoder for the stream's codec; streams for which no decoder can
 * be found are ignored.
 * 最佳流是根据各种启发法确定为最有可能是用户期望的流。 如果解码器参数非NULL，
 * av_find_best_stream将为流的编解码器查找默认解码器； 无法找到解码器的流将被忽略。
 *
 * @param ic                media file handle媒体文件句柄
 * @param type              stream type: video, audio, subtitles, etc.码流类型：视频、音频、字幕等
 * @param wanted_stream_nb  user-requested stream number,
 *                          or -1 for automatic selection
 *                          用户请求的流编号，或 -1 用于自动选择
 * @param related_stream    try to find a stream related (eg. in the same
 *                          program) to this one, or -1 if none
 *                          尝试找到与此相关的流（例如在同一节目中），如果没有则为 -1
 * @param decoder_ret       if non-NULL, returns the decoder for the
 *                          selected stream
 *                          如果非 NULL，则返回所选流的解码器
 * @param flags             flags; none are currently defined
 *                          标志，目前没有定义
 *
 * @return  the non-negative stream number in case of success,
 *          AVERROR_STREAM_NOT_FOUND if no stream with the requested type
 *          could be found,
 *          AVERROR_DECODER_NOT_FOUND if streams were found but no decoder
 * 如果成功，则为非负流编号；如果找不到具有请求类型的流，则为 AVERROR_STREAM_NOT_FOUND；
 * 如果找到流但没有解码器，则为 AVERROR_DECODER_NOT_FOUND
 *
 * @note  If av_find_best_stream returns successfully and decoder_ret is not
 *        NULL, then *decoder_ret is guaranteed to be set to a valid AVCodec.
 * 如果av_find_best_stream成功返回并且decoder_ret不为NULL，则保证*decoder_ret被设置为有效的AVCodec。
 */
int av_find_best_stream(AVFormatContext *ic,
                        enum AVMediaType type,
                        int wanted_stream_nb,
                        int related_stream,
                        const AVCodec **decoder_ret,
                        int flags);

/**
 * Return the next frame of a stream.
 * 返回流的下一帧。
 * This function returns what is stored in the file, and does not validate
 * that what is there are valid frames for the decoder. It will split what is
 * stored in the file into frames and return one for each call. It will not
 * omit invalid data between valid frames so as to give the decoder the maximum
 * information possible for decoding.
 * 此函数返回文件中存储的内容，并且不验证其中的内容是否是解码器的有效帧。
 * 它将文件中存储的内容分割成帧，并为每次调用返回一个帧。 它不会遗漏有效帧之间的无效数据，
 * 从而为解码器提供最大可能的解码信息。
 *
 * On success, the returned packet is reference-counted (pkt->buf is set) and
 * valid indefinitely. The packet must be freed with av_packet_unref() when
 * it is no longer needed. For video, the packet contains exactly one frame.
 * For audio, it contains an integer number of frames if each frame has
 * a known fixed size (e.g. PCM or ADPCM data). If the audio frames have
 * a variable size (e.g. MPEG audio), then it contains one frame.
 * 成功后，返回的数据包将进行引用计数（设置 pkt->buf）并且无限期有效。
 * 当不再需要数据包时，必须使用 av_packet_unref() 释放它。 对于视频，数据包仅包含一帧。
 * 对于音频，如果每个帧具有已知的固定大小（例如 PCM 或 ADPCM 数据），则它包含整数个帧。
 * 如果音频帧的大小可变（例如 MPEG 音频），则它包含一帧。
 *
 * pkt->pts, pkt->dts and pkt->duration are always set to correct
 * values in AVStream.time_base units (and guessed if the format cannot
 * provide them). pkt->pts can be AV_NOPTS_VALUE if the video format
 * has B-frames, so it is better to rely on pkt->dts if you do not
 * decompress the payload.
 * pkt->pts、pkt->dts 和 pkt->duration 始终设置为 AVStream.time_base 单位中的正确值
 * （并猜测如果格式无法提供它们）。 如果视频格式有 B 帧，pkt->pts 可以是 AV_NOPTS_VALUE，
 * 因此如果不解压缩有效负载，最好依赖 pkt->dts。
 *
 * @return 0 if OK, < 0 on error or end of file. On error, pkt will be blank
 *         (as if it came from av_packet_alloc()).
 * 如果正常则为 0，如果发生错误或文件结尾则为 < 0。 出错时，pkt 将为空（就好像它来自 av_packet_alloc()）。
 *
 * @note pkt will be initialized, so it may be uninitialized, but it must not
 *       contain data that needs to be freed.
 * pkt将被初始化，因此它可能未初始化，但它不能包含需要释放的数据。
 */
int av_read_frame(AVFormatContext *s, AVPacket *pkt);

/**
 * Seek to the keyframe at timestamp.
 * 查找时间戳处的关键帧。
 * 'timestamp' in 'stream_index'.
 * “stream_index”中的“时间戳”。
 *
 * @param s            media file handle媒体文件句柄
 * @param stream_index If stream_index is (-1), a default stream is selected,
 *                     and timestamp is automatically converted from
 *                     AV_TIME_BASE units to the stream specific time_base.
 * 如果stream_index为(-1)，则选择默认流，并且时间戳自动从AV_TIME_BASE单位转换为流特定的time_base。
 * @param timestamp    Timestamp in AVStream.time_base units or, if no stream
 *                     is specified, in AV_TIME_BASE units.
 * 时间戳以 AVStream.time_base 为单位，如果未指定流，则以 AV_TIME_BASE 为单位。
 * @param flags        flags which select direction and seeking mode
 * 选择方向和搜索模式的标志
 *
 * @return >= 0 on success
 */
int av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp,
                  int flags);

/**
 * Seek to timestamp ts.
 * 查找时间戳 ts。
 * Seeking will be done so that the point from which all active streams
 * can be presented successfully will be closest to ts and within min/max_ts.
 * Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
 * 将进行查找，以便可以成功呈现所有活动流的点将最接近 ts 并在 min/max_ts 之内。
 * 活动流是 AVStream.discard < AVDISCARD_ALL 的所有流。
 *
 * If flags contain AVSEEK_FLAG_BYTE, then all timestamps are in bytes and
 * are the file position (this may not be supported by all demuxers).
 * If flags contain AVSEEK_FLAG_FRAME, then all timestamps are in frames
 * in the stream with stream_index (this may not be supported by all demuxers).
 * Otherwise all timestamps are in units of the stream selected by stream_index
 * or if stream_index is -1, in AV_TIME_BASE units.
 * If flags contain AVSEEK_FLAG_ANY, then non-keyframes are treated as
 * keyframes (this may not be supported by all demuxers).
 * If flags contain AVSEEK_FLAG_BACKWARD, it is ignored.
 * 如果标志包含 AVSEEK_FLAG_BYTE，则所有时间戳均以字节为单位，并且是文件位置（这可能不受所有解复用器支持）。
 * 如果标志包含 AVSEEK_FLAG_FRAME，则所有时间戳都位于具有stream_index 的流中的帧中（这可能不受所有解复用器支持）。
 * 否则，所有时间戳均以stream_index选择的流为单位，或者如果stream_index为-1，则以AV_TIME_BASE为单位。
 * 如果标志包含 AVSEEK_FLAG_ANY，则非关键帧将被视为关键帧（这可能不受所有解复用器支持）。
 * 如果标志包含 AVSEEK_FLAG_BACKWARD，则会被忽略。
 *
 * @param s            media file handle媒体文件句柄
 * @param stream_index index of the stream which is used as time base reference
 * @param min_ts       smallest acceptable timestamp
 * @param ts           target timestamp
 * @param max_ts       largest acceptable timestamp
 * @param flags        flags
 * @return >=0 on success, error code otherwise
 *
 * @note This is part of the new seek API which is still under construction.
 * 这是仍在建设中的新搜索 API 的一部分。
 */
int avformat_seek_file(AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags);

/**
 * Discard all internally buffered data. This can be useful when dealing with
 * discontinuities in the byte stream. Generally works only with formats that
 * can resync. This includes headerless formats like MPEG-TS/TS but should also
 * work with NUT, Ogg and in a limited way AVI for example.
 * 丢弃所有内部缓冲的数据。
 * 这在处理字节流中的不连续性时非常有用。 通常仅适用于可以重新同步的格式。 这包括 MPEG-TS/TS
 * 等无标头格式，但也应与 NUT、Ogg 以及有限的 AVI 等一起使用。
 *
 * The set of streams, the detected duration, stream parameters and codecs do
 * not change when calling this function. If you want a complete reset, it's
 * better to open a new AVFormatContext.
 * 调用此函数时，流集、检测到的持续时间、流参数和编解码器不会更改。
 * 如果您想要完全重置，最好打开一个新的 AVFormatContext。
 *
 * This does not flush the AVIOContext (s->pb). If necessary, call
 * avio_flush(s->pb) before calling this function.
 * 这不会刷新 AVIOContext (s->pb)。 如有必要，请在调用此函数之前调用 avio_flush(s->pb)。
 *
 * @param s media file handle媒体文件句柄
 * @return >=0 on success, error code otherwise
 */
int avformat_flush(AVFormatContext *s);

/**
 * Start playing a network-based stream (e.g. RTSP stream) at the
 * current position.
 * 在当前位置开始播放基于网络的流（例如 RTSP 流）。
 */
int av_read_play(AVFormatContext *s);

/**
 * Pause a network-based stream (e.g. RTSP stream).
 * 暂停基于网络的流（例如 RTSP 流）。
 *
 * Use av_read_play() to resume it.
 */
int av_read_pause(AVFormatContext *s);

/**
 * Close an opened input AVFormatContext. Free it and all its contents
 * and set *s to NULL.
 * 关闭打开的输入 AVFormatContext。
 * 释放它及其所有内容并将 *s 设置为 NULL。
 */
void avformat_close_input(AVFormatContext **s);
/**
 * @}
 */

#define AVSEEK_FLAG_BACKWARD 1 ///< seek backward，向后seek
#define AVSEEK_FLAG_BYTE     2 ///< seeking based on position in bytes
                               ///< 基于字节位置进行查找
#define AVSEEK_FLAG_ANY      4 ///< seek to any frame, even non-keyframes
                               ///< 寻找任何帧，甚至非关键帧
#define AVSEEK_FLAG_FRAME    8 ///< seeking based on frame number
                               ///< 根据帧号查找

/**
 * @addtogroup lavf_encoding
 * @{
 */

#define AVSTREAM_INIT_IN_WRITE_HEADER 0 ///< stream parameters initialized in avformat_write_header
                                        ///< avformat_write_header 中初始化的流参数
#define AVSTREAM_INIT_IN_INIT_OUTPUT  1 ///< stream parameters initialized in avformat_init_output
                                        ///< avformat_init_output 中初始化的流参数

/**
 * Allocate the stream private data and write the stream header to
 * an output media file.
 * 分配流私有数据并将流头写入输出媒体文件。
 *
 * @param s        Media file handle, must be allocated with
 *                 avformat_alloc_context().
 *                 Its \ref AVFormatContext.oformat "oformat" field must be set
 *                 to the desired output format;
 *                 Its \ref AVFormatContext.pb "pb" field must be set to an
 *                 already opened ::AVIOContext.
 *                 媒体文件句柄，必须使用 avformat_alloc_context() 分配。
 *                 其oformat字段必须设置为所需的输出格式；
 *                 其 pb 字段必须设置为已打开的 AVIOContext。
 * @param options  An ::AVDictionary filled with AVFormatContext and
 *                 muxer-private options.
 *                 On return this parameter will be destroyed and replaced with
 *                 a dict containing options that were not found. May be NULL.
 *                 包含 AVFormatContext 和 muxer-private 选项的 AVDictionary。
 *                 返回时，此参数将被销毁并替换为包含未找到的选项的字典。 可能为 NULL。
 *
 * @retval AVSTREAM_INIT_IN_WRITE_HEADER On success, if the codec had not already been
 *                                       fully initialized in avformat_init_output().
 * 成功时，如果编解码器尚未在 avformat_init_output() 中完全初始化。
 * @retval AVSTREAM_INIT_IN_INIT_OUTPUT  On success, if the codec had already been fully
 *                                       initialized in avformat_init_output().
 * 如果编解码器已在 avformat_init_output() 中完全初始化，则成功。
 * @retval AVERROR                       A negative AVERROR on failure.
 *
 * @see av_opt_find, av_dict_set, avio_open, av_oformat_next, avformat_init_output.
 */
av_warn_unused_result
int avformat_write_header(AVFormatContext *s, AVDictionary **options);

/**
 * Allocate the stream private data and initialize the codec, but do not write the header.
 * 分配流私有数据并初始化编解码器，但不写入标头。
 * May optionally be used before avformat_write_header() to initialize stream parameters
 * before actually writing the header.
 * If using this function, do not pass the same options to avformat_write_header().
 * 可以选择在 avformat_write_header() 之前使用，以在实际写入标头之前初始化流参数。
 * 如果使用此函数，请勿将相同的选项传递给 avformat_write_header()。
 *
 * @param s        Media file handle, must be allocated with
 *                 avformat_alloc_context().
 *                 Its \ref AVFormatContext.oformat "oformat" field must be set
 *                 to the desired output format;
 *                 Its \ref AVFormatContext.pb "pb" field must be set to an
 *                 already opened ::AVIOContext.
 * 媒体文件句柄，必须使用 avformat_alloc_context() 分配。
 * 其oformat字段必须设置为所需的输出格式； 其 pb 字段必须设置为已打开的 AVIOContext。
 * @param options  An ::AVDictionary filled with AVFormatContext and
 *                 muxer-private options.
 *                 On return this parameter will be destroyed and replaced with
 *                 a dict containing options that were not found. May be NULL.
 * 包含 AVFormatContext 和 muxer-private 选项的 AVDictionary。
 * 返回时，此参数将被销毁并替换为包含未找到的选项的字典。 可能为 NULL。
 *
 * @retval AVSTREAM_INIT_IN_WRITE_HEADER On success, if the codec requires
 *                                       avformat_write_header to fully initialize.
 * 成功，如果编解码器需要 avformat_write_header 来完全初始化。
 * @retval AVSTREAM_INIT_IN_INIT_OUTPUT  On success, if the codec has been fully
 *                                       initialized.
 * 成功，如果编解码器已完全初始化。
 * @retval AVERROR                       Anegative AVERROR on failure.
 *
 * @see av_opt_find, av_dict_set, avio_open, av_oformat_next, avformat_write_header.
 */
av_warn_unused_result
int avformat_init_output(AVFormatContext *s, AVDictionary **options);

/**
 * Write a packet to an output media file.
 * 将数据包写入输出媒体文件。
 *
 * This function passes the packet directly to the muxer, without any buffering
 * or reordering. The caller is responsible for correctly interleaving the
 * packets if the format requires it. Callers that want libavformat to handle
 * the interleaving should call av_interleaved_write_frame() instead of this
 * function.
 * 此函数将数据包直接传递到复用器，无需任何缓冲或重新排序。 如果格式需要，调用者负责正确交错数据包。
 * 希望 libavformat 处理交错的调用者应该调用 av_interleaved_write_frame() 而不是此函数。
 *
 * @param s media file handle媒体文件句柄
 * @param pkt The packet containing the data to be written. Note that unlike
 *            av_interleaved_write_frame(), this function does not take
 *            ownership of the packet passed to it (though some muxers may make
 *            an internal reference to the input packet).
 *            包含要写入的数据的数据包。 请注意，与 av_interleaved_write_frame() 不同，
 *            此函数不获取传递给它的数据包的所有权（尽管某些复用器可能会对输入数据包进行内部引用）。
 *            <br>
 *            This parameter can be NULL (at any time, not just at the end), in
 *            order to immediately flush data buffered within the muxer, for
 *            muxers that buffer up data internally before writing it to the
 *            output.
 *            此参数可以为 NULL（任何时候，而不仅仅是在最后），
 *            以便立即刷新复用器内缓冲的数据，对于在将数据写入输出之前在内部缓冲数据的复用器。
 *            <br>
 *            Packet's @ref AVPacket.stream_index "stream_index" field must be
 *            set to the index of the corresponding stream in @ref
 *            AVFormatContext.streams "s->streams".
 *            数据包的stream_index字段必须设置为s->streams中相应流的索引。
 *            <br>
 *            The timestamps (@ref AVPacket.pts "pts", @ref AVPacket.dts "dts")
 *            must be set to correct values in the stream's timebase (unless the
 *            output format is flagged with the AVFMT_NOTIMESTAMPS flag, then
 *            they can be set to AV_NOPTS_VALUE).
 *            The dts for subsequent packets passed to this function must be strictly
 *            increasing when compared in their respective timebases (unless the
 *            output format is flagged with the AVFMT_TS_NONSTRICT, then they
 *            merely have to be nondecreasing).  @ref AVPacket.duration
 *            "duration") should also be set if known.
 *            时间戳（pts、dts）必须设置为流时基中的正确值（除非输出格式标记有 AVFMT_NOTIMESTAMPS 标志，
 *            则可以将它们设置为 AV_NOPTS_VALUE）。 与各自的时基相比，传递到此函数的后续数据包的 dts
 *            必须严格递增（除非输出格式标记有 AVFMT_TS_NONSTRICT，则它们只需非递减）。 如果已知，还应设置持续时间）。
 * @return < 0 on error, = 0 if OK, 1 if flushed and there is no more data to flush
 *
 * @see av_interleaved_write_frame()
 */
int av_write_frame(AVFormatContext *s, AVPacket *pkt);

/**
 * Write a packet to an output media file ensuring correct interleaving.
 * 将数据包写入输出媒体文件，确保正确交错。
 *
 * This function will buffer the packets internally as needed to make sure the
 * packets in the output file are properly interleaved, usually ordered by
 * increasing dts. Callers doing their own interleaving should call
 * av_write_frame() instead of this function.
 * 该函数将根据需要在内部缓冲数据包，以确保输出文件中的数据包正确交错，
 * 通常通过增加 dts 进行排序。 进行自己的交错的调用者应该调用 av_write_frame() 而不是此函数。
 *
 * Using this function instead of av_write_frame() can give muxers advance
 * knowledge of future packets, improving e.g. the behaviour of the mp4
 * muxer for VFR content in fragmenting mode.
 * 使用此函数代替 av_write_frame() 可以让复用器预先了解未来的数据包，
 * 从而改进例如 分段模式下 VFR 内容的 mp4 复用器的行为。
 *
 * @param s media file handle媒体文件句柄
 * @param pkt The packet containing the data to be written.
 *             包含要写入的数据的数据包。
 *            <br>
 *            If the packet is reference-counted, this function will take
 *            ownership of this reference and unreference it later when it sees
 *            fit. If the packet is not reference-counted, libavformat will
 *            make a copy.
 *            The returned packet will be blank (as if returned from
 *            av_packet_alloc()), even on error.
 *            如果数据包是引用计数的，则该函数将取得该引用的所有权，并在以后认为合适时取消引用它。
 *            如果数据包不是引用计数的，libavformat 将制作一个副本。 即使出现错误，返回的数据包也将为空白
 *           （就像从 av_packet_alloc() 返回一样）。
 *            <br>
 *            This parameter can be NULL (at any time, not just at the end), to
 *            flush the interleaving queues.
 *            该参数可以为 NULL（任何时候，而不仅仅是在最后），以刷新交错队列。
 *            <br>
 *            Packet's @ref AVPacket.stream_index "stream_index" field must be
 *            set to the index of the corresponding stream in @ref
 *            AVFormatContext.streams "s->streams".
 *            数据包的stream_index字段必须设置为s->streams中相应流的索引。
 *            <br>
 *            The timestamps (@ref AVPacket.pts "pts", @ref AVPacket.dts "dts")
 *            must be set to correct values in the stream's timebase (unless the
 *            output format is flagged with the AVFMT_NOTIMESTAMPS flag, then
 *            they can be set to AV_NOPTS_VALUE).
 *            The dts for subsequent packets in one stream must be strictly
 *            increasing (unless the output format is flagged with the
 *            AVFMT_TS_NONSTRICT, then they merely have to be nondecreasing).
 *            @ref AVPacket.duration "duration" should also be set if known.
 *            时间戳（pts、dts）必须设置为流时基中的正确值（除非输出格式标记有 AVFMT_NOTIMESTAMPS 标志，
 *            否则可以将它们设置为 AV_NOPTS_VALUE）。 一个流中后续数据包的 dts 必须严格递增（除非输出格式
 *            标记有 AVFMT_TS_NONSTRICT，则它们只需为非递减）。 如果已知，还应设置持续时间。
 *
 * @return 0 on success, a negative AVERROR on error.
 *
 * @see av_write_frame(), AVFormatContext.max_interleave_delta
 */
int av_interleaved_write_frame(AVFormatContext *s, AVPacket *pkt);

/**
 * Write an uncoded frame to an output media file.
 * 将未编码的帧写入输出媒体文件。
 *
 * The frame must be correctly interleaved according to the container
 * specification; if not, av_interleaved_write_uncoded_frame() must be used.
 * 改帧必须按照容器规格正确交错； 如果不是，则必须使用 av_interleaved_write_uncoded_frame()。
 *
 * See av_interleaved_write_uncoded_frame() for details.
 */
int av_write_uncoded_frame(AVFormatContext *s, int stream_index,
                           AVFrame *frame);

/**
 * Write an uncoded frame to an output media file.
 * 将未编码的帧写入输出媒体文件。
 *
 * If the muxer supports it, this function makes it possible to write an AVFrame
 * structure directly, without encoding it into a packet.
 * It is mostly useful for devices and similar special muxers that use raw
 * video or PCM data and will not serialize it into a byte stream.
 * 如果复用器支持，此函数可以直接编写 AVFrame 结构，而无需将其编码到数据包中。
 * 它对于使用原始视频或 PCM 数据并且不会将其序列化为字节流的设备和类似的特殊复用器最有用。
 *
 * To test whether it is possible to use it with a given muxer and stream,
 * use av_write_uncoded_frame_query().
 * 要测试是否可以将其与给定的复用器和流一起使用，请使用 av_write_uncoded_frame_query()。
 *
 * The caller gives up ownership of the frame and must not access it
 * afterwards.
 * 调用者放弃该帧的所有权，并且之后不得访问它。
 *
 * @return  >=0 for success, a negative code on error
 */
int av_interleaved_write_uncoded_frame(AVFormatContext *s, int stream_index,
                                       AVFrame *frame);

/**
 * Test whether a muxer supports uncoded frame.
 * 测试复用器是否支持未编码帧。
 *
 * @return  >=0 if an uncoded frame can be written to that muxer and stream,
 *          <0 if not
 * 如果可以将未编码帧写入该复用器和流，则 >=0，如果不能，则 <0
 */
int av_write_uncoded_frame_query(AVFormatContext *s, int stream_index);

/**
 * Write the stream trailer to an output media file and free the
 * file private data.
 * 将流尾写入输出媒体文件并释放文件的私有数据。
 *
 * May only be called after a successful call to avformat_write_header.
 * 只能在成功调用avformat_write_header之后调用。
 *
 * @param s media file handle
 * @return 0 if OK, AVERROR_xxx on error
 */
int av_write_trailer(AVFormatContext *s);

/**
 * Return the output format in the list of registered output formats
 * which best matches the provided parameters, or return NULL if
 * there is no match.
 * 返回注册输出格式列表中与提供的参数最匹配的输出格式，如果不匹配则返回 NULL。
 *
 * @param short_name if non-NULL checks if short_name matches with the
 *                   names of the registered formats
 * if non-NULL 检查short_name是否与注册格式的名称匹配
 * @param filename   if non-NULL checks if filename terminates with the
 *                   extensions of the registered formats
 * if non-NULL 检查文件名是否以注册格式的扩展名结尾
 * @param mime_type  if non-NULL checks if mime_type matches with the
 *                   MIME type of the registered formats
 * if non-NULL 检查 mime_type 是否与注册格式的 MIME 类型匹配
 */
const AVOutputFormat *av_guess_format(const char *short_name,
                                      const char *filename,
                                      const char *mime_type);

/**
 * Guess the codec ID based upon muxer and filename.
 * 根据复用器和文件名猜测编解码器 ID。
 */
enum AVCodecID av_guess_codec(const AVOutputFormat *fmt, const char *short_name,
                              const char *filename, const char *mime_type,
                              enum AVMediaType type);

/**
 * Get timing information for the data currently output.
 * 获取当前输出数据的时序信息。
 * The exact meaning of "currently output" depends on the format.
 * It is mostly relevant for devices that have an internal buffer and/or
 * work in real time.
 * “当前输出”的确切含义取决于格式。 它主要与具有内部缓冲区和/或实时工作的设备相关。
 * @param s          media file handle媒体文件句柄
 * @param stream     stream in the media file媒体文件中的流
 * @param[out] dts   DTS of the last packet output for the stream, in stream
 *                   time_base units
 * 流的最后一个数据包输出的 DTS（以流 time_base 为单位）
 * @param[out] wall  absolute time when that packet whas output,
 *                   in microsecond
 * 该数据包输出的绝对时间，以微秒为单位
 * @retval  0               Success
 * @retval  AVERROR(ENOSYS) The format does not support it
 *
 * @note Some formats or devices may not allow to measure dts and wall
 *       atomically.
 * 某些格式或设备可能不允许自动测量 dts 和 wall。
 */
int av_get_output_timestamp(struct AVFormatContext *s, int stream,
                            int64_t *dts, int64_t *wall);


/**
 * @}
 */


/**
 * @defgroup lavf_misc Utility functions
 * @ingroup libavf
 * @{
 *
 * Miscellaneous utility functions related to both muxing and demuxing
 * (or neither).
 * 与复用和解复用（或两者都不相关）相关的杂项实用函数。
 */

/**
 * Send a nice hexadecimal dump of a buffer to the specified file stream.
 * 将缓冲区的十六进制转储发送到指定的文件流。
 *
 * @param f The file stream pointer where the dump should be sent to.
 * 转储应发送到的文件流指针。
 * @param buf buffer
 * @param size buffer size
 *
 * @see av_hex_dump_log, av_pkt_dump2, av_pkt_dump_log2
 */
void av_hex_dump(FILE *f, const uint8_t *buf, int size);

/**
 * Send a nice hexadecimal dump of a buffer to the log.
 * 将缓冲区的十六进制转储发送到日志。
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 * pointer to an AVClass struct.
 * 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针。
 * @param level The importance level of the message, lower values signifying
 * higher importance.
 * 消息的重要性级别，较低的值表示较高的重要性。
 * @param buf buffer
 * @param size buffer size
 *
 * @see av_hex_dump, av_pkt_dump2, av_pkt_dump_log2
 */
void av_hex_dump_log(void *avcl, int level, const uint8_t *buf, int size);

/**
 * Send a nice dump of a packet to the specified file stream.
 * 将数据包的良好转储发送到指定的文件流。
 *
 * @param f The file stream pointer where the dump should be sent to.
 * 转储应发送到的文件流指针。
 * @param pkt packet to dump
 * 要转储的数据包
 * @param dump_payload True if the payload must be displayed, too.
 * 如果还必须显示有效负载，则为 True。
 * @param st AVStream that the packet belongs to
 * 数据包所属的AVStream
 */
void av_pkt_dump2(FILE *f, const AVPacket *pkt, int dump_payload, const AVStream *st);


/**
 * Send a nice dump of a packet to the log.
 * 将数据包的良好转储发送到日志。
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 * pointer to an AVClass struct.
 * 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针。
 * @param level The importance level of the message, lower values signifying
 * higher importance.
 * 消息的重要性级别，较低的值表示较高的重要性。
 * @param pkt packet to dump
 * 要转储的数据包
 * @param dump_payload True if the payload must be displayed, too.
 * 如果还必须显示有效负载，则为 True。
 * @param st AVStream that the packet belongs to
 * 数据包所属的AVStream
 */
void av_pkt_dump_log2(void *avcl, int level, const AVPacket *pkt, int dump_payload,
                      const AVStream *st);

/**
 * Get the AVCodecID for the given codec tag tag.
 * 获取给定编解码器标签的 AVCodecID。
 * If no codec id is found returns AV_CODEC_ID_NONE.
 * 如果未找到编解码器 ID，则返回 AV_CODEC_ID_NONE。
 *
 * @param tags list of supported codec_id-codec_tag pairs, as stored
 * in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
 * 支持的 codec_id-codec_tag 对列表，存储在 AVInputFormat.codec_tag 和 AVOutputFormat.codec_tag 中
 * @param tag  codec tag to match to a codec ID
 * 与编解码器 ID 匹配的编解码器标签
 */
enum AVCodecID av_codec_get_id(const struct AVCodecTag * const *tags, unsigned int tag);

/**
 * Get the codec tag for the given codec id id.
 * 获取给定编解码器 ID 的编解码器标签。
 * If no codec tag is found returns 0.
 * 如果未找到编解码器标签，则返回 0。
 *
 * @param tags list of supported codec_id-codec_tag pairs, as stored
 * in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
 * 支持的 codec_id-codec_tag 对列表，存储在 AVInputFormat.codec_tag 和 AVOutputFormat.codec_tag 中
 * @param id   codec ID to match to a codec tag
 * 与编解码器标签匹配的编解码器 ID
 */
unsigned int av_codec_get_tag(const struct AVCodecTag * const *tags, enum AVCodecID id);

/**
 * Get the codec tag for the given codec id.
 * 获取给定编解码器 ID 的编解码器标签。
 *
 * @param tags list of supported codec_id - codec_tag pairs, as stored
 * in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
 * 支持的 codec_id - codec_tag 对列表，存储在 AVInputFormat.codec_tag 和 AVOutputFormat.codec_tag 中
 * @param id codec id that should be searched for in the list
 * 应在列表中搜索的编解码器 ID
 * @param tag A pointer to the found tag
 * 指向找到的标签的指针
 * @return 0 if id was not found in tags, > 0 if it was found
 * 如果在标签中找不到 id，则为 0；如果找到，则 > 0
 */
int av_codec_get_tag2(const struct AVCodecTag * const *tags, enum AVCodecID id,
                      unsigned int *tag);

int av_find_default_stream_index(AVFormatContext *s);

/**
 * Get the index for a specific timestamp.
 * 获取特定时间戳的索引。
 *
 * @param st        stream that the timestamp belongs to
 * 时间戳所属的流
 * @param timestamp timestamp to retrieve the index for
 * 检索索引的时间戳
 * @param flags if AVSEEK_FLAG_BACKWARD then the returned index will correspond
 *                 to the timestamp which is <= the requested one, if backward
 *                 is 0, then it will be >=
 *              if AVSEEK_FLAG_ANY seek to any frame, only keyframes otherwise
 * 如果AVSEEK_FLAG_BACKWARD则返回的索引将对应于<=请求的时间戳，如果backward为0，则它将>=
 * 如果 AVSEEK_FLAG_ANY 寻找任何帧，否则仅寻找关键帧
 * @return < 0 if no such timestamp could be found
 * < 0 如果找不到这样的时间戳
 */
int av_index_search_timestamp(AVStream *st, int64_t timestamp, int flags);

/**
 * Get the index entry count for the given AVStream.
 * 获取给定 AVStream 的索引条目计数。
 *
 * @param st stream
 * @return the number of index entries in the stream
 * 流中索引条目的数量
 */
int avformat_index_get_entries_count(const AVStream *st);

/**
 * Get the AVIndexEntry corresponding to the given index.
 * 获取给定索引对应的AVIndexEntry。
 *
 * @param st          Stream containing the requested AVIndexEntry.
 * 包含所请求的 AVIndexEntry 的流。
 * @param idx         The desired index.
 * 所需的索引。
 * @return A pointer to the requested AVIndexEntry if it exists, NULL otherwise.
 * 指向所请求的 AVIndexEntry 的指针（如果存在），否则为 NULL。
 *
 * @note The pointer returned by this function is only guaranteed to be valid
 *       until any function that takes the stream or the parent AVFormatContext
 *       as input argument is called.
 * 仅保证此函数返回的指针有效，直到调用任何将流或父级 AVFormatContext 作为输入参数的函数为止。
 */
const AVIndexEntry *avformat_index_get_entry(AVStream *st, int idx);

/**
 * Get the AVIndexEntry corresponding to the given timestamp.
 * 获取给定时间戳对应的AVIndexEntry。
 *
 * @param st          Stream containing the requested AVIndexEntry.
 * 包含所请求的 AVIndexEntry 的流。
 * @param wanted_timestamp   Timestamp to retrieve the index entry for.
 * 用于检索索引条目的时间戳。
 * @param flags       If AVSEEK_FLAG_BACKWARD then the returned entry will correspond
 *                    to the timestamp which is <= the requested one, if backward
 *                    is 0, then it will be >=
 *                    if AVSEEK_FLAG_ANY seek to any frame, only keyframes otherwise.
 * 如果AVSEEK_FLAG_BACKWARD则返回的条目将对应于<=请求的时间戳，如果向后为0，则它将>=
 * 如果 AVSEEK_FLAG_ANY 寻找任何帧，否则仅寻找关键帧。
 * @return A pointer to the requested AVIndexEntry if it exists, NULL otherwise.
 * 指向所请求的 AVIndexEntry 的指针（如果存在），否则为 NULL。
 *
 * @note The pointer returned by this function is only guaranteed to be valid
 *       until any function that takes the stream or the parent AVFormatContext
 *       as input argument is called.
 * 仅保证此函数返回的指针有效，直到调用任何将流或父级 AVFormatContext 作为输入参数的函数为止。
 */
const AVIndexEntry *avformat_index_get_entry_from_timestamp(AVStream *st,
                                                            int64_t wanted_timestamp,
                                                            int flags);
/**
 * Add an index entry into a sorted list. Update the entry if the list
 * already contains it.
 * 将索引条目添加到排序列表中。如果列表已包含该条目，则更新该条目。
 *
 * @param timestamp timestamp in the time base of the given stream
 * 给定流的时基中的时间戳
 */
int av_add_index_entry(AVStream *st, int64_t pos, int64_t timestamp,
                       int size, int distance, int flags);


/**
 * Split a URL string into components.
 * 将 URL 字符串拆分为多个组件。
 *
 * The pointers to buffers for storing individual components may be null,
 * in order to ignore that component. Buffers for components not found are
 * set to empty strings. If the port is not found, it is set to a negative
 * value.
 * 指向用于存储各个组件的缓冲区的指针可以为空，以便忽略该组件。
 * 未找到的组件的缓冲区设置为空字符串。
 * 如果未找到该端口，则将其设置为负值。
 *
 * @param proto the buffer for the protocol，协议缓冲区
 * @param proto_size the size of the proto buffer，协议缓冲区的大小
 * @param authorization the buffer for the authorization，授权缓冲区
 * @param authorization_size the size of the authorization buffer，授权缓冲区的大小
 * @param hostname the buffer for the host name，主机名的缓冲区
 * @param hostname_size the size of the hostname buffer，主机名的缓冲区的大小
 * @param port_ptr a pointer to store the port number in，存储端口号的指针
 * @param path the buffer for the path，路径的缓冲区
 * @param path_size the size of the path buffer，路径缓冲区的大小
 * @param url the URL to split，要分割的 URL
 */
void av_url_split(char *proto,         int proto_size,
                  char *authorization, int authorization_size,
                  char *hostname,      int hostname_size,
                  int *port_ptr,
                  char *path,          int path_size,
                  const char *url);


/**
 * Print detailed information about the input or output format, such as
 * duration, bitrate, streams, container, programs, metadata, side data,
 * codec and time base.
 * 打印有关输入或输出格式的详细信息，例如持续时间、比特率、流、容器、程序、元数据、辅助数据、编解码器和时基。
 *
 * @param ic        the context to analyze，要分析的上下文
 * @param index     index of the stream to dump information about，要转储信息的流的索引
 * @param url       the URL to print, such as source or destination file，要打印的 URL，例如源文件或目标文件
 * @param is_output Select whether the specified context is an input(0) or output(1)，选择指定的上下文是输入（0）还是输出（1）
 */
void av_dump_format(AVFormatContext *ic,
                    int index,
                    const char *url,
                    int is_output);


#define AV_FRAME_FILENAME_FLAGS_MULTIPLE 1 ///< Allow multiple %d

/**
 * Return in 'buf' the path with '%d' replaced by a number.
 *在“buf”中返回路径，并将“d”替换为数字。
 * Also handles the '%0nd' format where 'n' is the total number
 * of digits and '%%'.
 * 还处理“%0nd”格式，其中“n”是数字总位数和“%”。
 *
 * @param buf destination buffer，目标缓冲区
 * @param buf_size destination buffer size，目标缓冲区大小
 * @param path numbered sequence string，编号序列字符串
 * @param number frame number，帧数
 * @param flags AV_FRAME_FILENAME_FLAGS_*
 * @return 0 if OK, -1 on format error
 */
int av_get_frame_filename2(char *buf, int buf_size,
                          const char *path, int number, int flags);

int av_get_frame_filename(char *buf, int buf_size,
                          const char *path, int number);

/**
 * Check whether filename actually is a numbered sequence generator.
 * 检查文件名是否实际上是编号序列生成器。
 *
 * @param filename possible numbered sequence string
 * 文件名 可能的编号序列字符串
 * @return 1 if a valid numbered sequence string, 0 otherwise
 * 如果是有效的编号序列字符串，则为 1，否则为 0
 */
int av_filename_number_test(const char *filename);

/**
 * Generate an SDP for an RTP session.
 * 为 RTP 会话生成 SDP。
 *
 * Note, this overwrites the id values of AVStreams in the muxer contexts
 * for getting unique dynamic payload types.
 * 请注意，这会覆盖复用器上下文中 AVStreams 的 id 值，以获取唯一的动态负载类型。
 *
 * @param ac array of AVFormatContexts describing the RTP streams. If the
 *           array is composed by only one context, such context can contain
 *           multiple AVStreams (one AVStream per RTP stream). Otherwise,
 *           all the contexts in the array (an AVCodecContext per RTP stream)
 *           must contain only one AVStream.
 * 描述 RTP 流的 AVFormatContext 数组。 如果该数组仅由一个上下文组成，
 * 则该上下文可以包含多个 AVStream（每个 RTP 流一个 AVStream）。 否则，
 * 数组中的所有上下文（每个 RTP 流一个 AVCodecContext）必须仅包含一个 AVStream。
 * @param n_files number of AVCodecContexts contained in ac
 * @param buf buffer where the SDP will be stored (must be allocated by
 *            the caller)
 * @param size the size of the buffer
 * @return 0 if OK, AVERROR_xxx on error
 */
int av_sdp_create(AVFormatContext *ac[], int n_files, char *buf, int size);

/**
 * Return a positive value if the given filename has one of the given
 * extensions, 0 otherwise.
 * 如果给定文件名具有给定扩展名之一，则返回正值，否则返回 0。
 *
 * @param filename   file name to check against the given extensions
 * 用于检查给定扩展名的文件名
 * @param extensions a comma-separated list of filename extensions
 * 以逗号分隔的文件扩展名列表
 */
int av_match_ext(const char *filename, const char *extensions);

/**
 * Test if the given container can store a codec.
 * 测试给定容器是否可以存储编解码器。
 *
 * @param ofmt           container to check for compatibility
 * 用于检查兼容性的容器
 * @param codec_id       codec to potentially store in container
 * 编解码器可能存储在容器中
 * @param std_compliance standards compliance level, one of FF_COMPLIANCE_*
 * 标准合规级别，FF_COMPLIANCE_* 之一
 *
 * @return 1 if codec with ID codec_id can be stored in ofmt, 0 if it cannot.
 *         A negative number if this information is not available.
 * 如果 ID 为 codec_id 的编解码器可以存储在 ofmt 中，则为 1；如果不能，则为 0。
 * 如果此信息不可用，则为负数。
 */
int avformat_query_codec(const AVOutputFormat *ofmt, enum AVCodecID codec_id,
                         int std_compliance);

/**
 * @defgroup riff_fourcc RIFF FourCCs
 * @{
 * Get the tables mapping RIFF FourCCs to libavcodec AVCodecIDs. The tables are
 * meant to be passed to av_codec_get_id()/av_codec_get_tag() as in the
 * following code:
 * 获取将 RIFF FourCC 映射到 libavcodec AVCodecID 的表。
 * 这些表旨在传递给 av_codec_get_id()/av_codec_get_tag()，如以下代码所示：
 * @code
 * uint32_t tag = MKTAG('H', '2', '6', '4');
 * const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), 0 };
 * enum AVCodecID id = av_codec_get_id(table, tag);
 * @endcode
 */
/**
 * @return the table mapping RIFF FourCCs for video to libavcodec AVCodecID.
 * 将视频的 RIFF FourCC 映射到 libavcodec AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_riff_video_tags(void);
/**
 * @return the table mapping RIFF FourCCs for audio to AVCodecID.
 * 将音频的 RIFF FourCC 映射到 AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_riff_audio_tags(void);
/**
 * @return the table mapping MOV FourCCs for video to libavcodec AVCodecID.
 * 将视频 MOV FourCC 映射到 libavcodec AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_mov_video_tags(void);
/**
 * @return the table mapping MOV FourCCs for audio to AVCodecID.
 * 将音频 MOV FourCC 映射到 AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_mov_audio_tags(void);

/**
 * @}
 */

/**
 * Guess the sample aspect ratio of a frame, based on both the stream and the
 * frame aspect ratio.
 * 根据流和帧长宽比猜测帧的样本长宽比。
 *
 * Since the frame aspect ratio is set by the codec but the stream aspect ratio
 * is set by the demuxer, these two may not be equal. This function tries to
 * return the value that you should use if you would like to display the frame.
 * 由于帧长宽比是由编解码器设置的，而流长宽比是由解复用器设置的，
 * 因此这两者可能不相等。 如果您想显示帧，此函数会尝试返回您应该使用的值。
 *
 * Basic logic is to use the stream aspect ratio if it is set to something sane
 * otherwise use the frame aspect ratio. This way a container setting, which is
 * usually easy to modify can override the coded value in the frames.
 * 基本逻辑是，如果流宽高比设置合理，则使用流宽高比，否则使用帧宽高比。
 * 这样，通常很容易修改的容器设置可以覆盖帧中的编码值。
 *
 * @param format the format context which the stream is part of
 * 流所属的格式上下文
 * @param stream the stream which the frame is part of
 * 该帧所属的流
 * @param frame the frame with the aspect ratio to be determined
 * 长宽比待确定的帧
 * @return the guessed (valid) sample_aspect_ratio, 0/1 if no idea
 * 猜测的（有效）sample_aspect_ratio，如果不知道则为 0/1
 */
AVRational av_guess_sample_aspect_ratio(AVFormatContext *format, AVStream *stream, AVFrame *frame);

/**
 * Guess the frame rate, based on both the container and codec information.
 * 根据容器和编解码器信息猜测帧速率。
 *
 * @param ctx the format context which the stream is part of
 * 流所属的格式上下文
 * @param stream the stream which the frame is part of
 * 该帧所属的流
 * @param frame the frame for which the frame rate should be determined, may be NULL
 * 应确定帧速率的帧可以为 NULL
 * @return the guessed (valid) frame rate, 0/1 if no idea
 * 猜测的（有效）帧速率，如果不知道则为 0/1
 */
AVRational av_guess_frame_rate(AVFormatContext *ctx, AVStream *stream, AVFrame *frame);

/**
 * Check if the stream st contained in s is matched by the stream specifier
 * spec.
 * 检查 s 中包含的流 st 是否与流说明符规范匹配。
 *
 * See the "stream specifiers" chapter in the documentation for the syntax
 * of spec.
 * 有关规范语法的信息，请参阅文档中的“流说明符”一章。
 *
 * @return  >0 if st is matched by spec;
 *          >0 如果 st 与 spec 匹配；
 *          0  if st is not matched by spec;
 *          如果 st 与 spec 不匹配，则为 0；
 *          AVERROR code if spec is invalid
 *          如果规格无效，则 AVERROR 代码
 *
 * @note  A stream specifier can match several streams in the format.
 * 流说明符可以匹配格式中的多个流。
 */
int avformat_match_stream_specifier(AVFormatContext *s, AVStream *st,
                                    const char *spec);

int avformat_queue_attached_pictures(AVFormatContext *s);

enum AVTimebaseSource {
    AVFMT_TBCF_AUTO = -1,
    AVFMT_TBCF_DECODER,
    AVFMT_TBCF_DEMUXER,
#if FF_API_R_FRAME_RATE
    AVFMT_TBCF_R_FRAMERATE,
#endif
};

/**
 * Transfer internal timing information from one stream to another.
 * 将内部计时信息从一个流传输到另一流。
 * This function is useful when doing stream copy.
 * 该功能在进行流复制时很有用。
 *
 * @param ofmt     target output format for ost
 * ost 的目标输出格式
 * @param ost      output stream which needs timings copy and adjustments
 * 需要定时复制和调整的输出流
 * @param ist      reference input stream to copy timings from
 * 参考输入流来复制时序
 * @param copy_tb  define from where the stream codec timebase needs to be imported
 * 定义需要从何处导入流编解码器时基
 */
int avformat_transfer_internal_stream_timing_info(const AVOutputFormat *ofmt,
                                                  AVStream *ost, const AVStream *ist,
                                                  enum AVTimebaseSource copy_tb);

/**
 * Get the internal codec timebase from a stream.
 * 从流中获取内部编解码器时基。
 *
 * @param st  input stream to extract the timebase from
 * 用于从中提取时基的输入流
 */
AVRational av_stream_get_codec_timebase(const AVStream *st);

/**
 * @}
 */

#endif /* AVFORMAT_AVFORMAT_H */

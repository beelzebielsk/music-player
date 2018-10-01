- For package manager like qualities, where you don't redownload things
	if not necessary, see the `--download-archive` option of youtube-dl.
- Places to search for tracklist information:
	- Comments (sometimes). Not sure if I can download a comment.
	- The `chapters` field of a video. Could be `null`, indicating no
		information.
	- The video description.
	- No information may be available.
- Places to search for the artist/album information;
	- The keywords might be a candidate.
- Places to figure out if the video is an album or track
	- If the keywords include "full album" or "full" and "album"
		separately.
	- If the title says "full album"
	- If the video is not a full album, then it is a track.

Downloading a Playlist
======================

- Get a URL for the playlist.
- Download the videos and keep only the audio.
- Download the json metadata for the videos.
- Try to extract as much information from the metadata as possible about
	the videos.
	- For each desired bit of information, you've got an object with two
		properties:
		- default value, which is the value to return if none of the sources
			return a value that is not `None`.
		- sources, which is a list of functions that take the metadata of
			the video and do something with it to return some appropriate
			data. Earlier sources take priority over later sources, and if the
			source doesn't find a useful or valid value, then the source
			should return `None`. If all the sources return `None`, then the
			default value will be consulted. If there is no default value,
			then the default is `None`.
	- Get a title for the video, that should be easy enough.
	- See if you can get some artist information about the video.
	- Figure out if the video is an album or a track.
		- If album, See if you can get some track information about the
			video.

TODO
====

Downloading a Single Video
--------------------------

- Download a video that's an album and split it using track information,
	if track information is available.
	- First, figure out if the track list is a list of durations, or a
		list of ends (where it is assumed that each track begins where the
		other track ends.
		- To detect this, I've got a few ideas:
		- Figure out if the last track ends when (or almost when) the video
			ends. This should be a surefire way, though I'd have to decide
			what's "close enough" to the end of the video. I'm thinking 99%
			percent of the way.
		- Check to see if the times listed are monotonically increasing.
			This must be the case for a list of end times (and is potentially
			possible for a list of durations, but I've never seen that).
	- Then, figure out how to programatically split a file at a given
		time.
		- If you come up with a "crop method", where you can remove a given
			section of a song, then just use the times to come up with a
			region for cropping. This is easiest with a list of starts/ends,
			because those are absolute positions in the video for the
			starts/ends.
		- If you come up with a "split into two at this area", then you'd
			probably have to do successive splits on smaller and smaller
			pieces.

Differentating Between A Video And Playlist
-------------------------------------------

- This shouldn't be too hard. I'm pretty sure youtube-dl does this on
  its own.

Downloading a Playlist
----------------------

- If I put in a playlist, then I should:
    - Go through each song in the playlist.
    - Figure out if the track has already been downloaded
        - If not already downloaded, then create an entry in a master
          downloads list with music information about the track.
        - The music information should specify exactly what to do with
          the track to get the desired result. Good musical metadata
          is a secondary concern as there's already programs/devices
          for getting that done right.

Creating a basic metadeta fetcher
---------------------------------

The metadata that comes with the video is really about the video, not
about the music in the video. So try to get important metadata about the
track in the video.

- Create machinery for the data fetcher.
	- The fetching information is a dictionary with the key being the
		information that you want and the value being another dictionary
		with necessary stuff for fetching the information, such as default
		value and list of sources/methods to get the information. The result
		will look almost the same: the a dictionary whose keys are the
		information and whose values are the fetched values.
- Figure out how to store all of that information in another file so
	that you don't have to redo it. That's basically the basis for the
	package manager like functionality. You store all of the information
	necessary for appropriately downloading and prepping the music.
- Figure out how to go from URLs to mindless downloading. I don't want
	to manually manage too much, and I don't want to think too much about
	where things go.

Support more than one source
----------------------------

- Figure out how things differ from source to source. Perhaps there's
	different metadata, perhaps the metadata gets structured differently.

Updated Project Ideas
=====================

Global watch later list. I can put music into it from a variety of
sources (various streaming links like youtube, reference to a local
file at home, song from a torrent, directly specify a song/album name
and have the program automatically find a source for that song/album
if available). Make these things available offline on the device that
I'm currently using to listen to music.

Have access to the same music collection across several devices.

I don't want to make my own music player. VLC does that well enough
already. So does XMMS2.

I don't want to put my whole music collection on each device. They
have limited storage. I want some form of music caching. The most
recent/most frequently listened to stuff stays on the device, but the
other stuff gets moved out as some storage limit is hit.

If I make a change to my collection from one device, all devices
should get it. So if I update my watch later list from my phone, then
the next time I turn on my computer, I should see updates to the watch
later list.

If one device is connected to another device, then those two devices
should update each other's information. So, going back to the updating
my watch later list from my phone. If I head home and turn on my
computer, then my computer should get the updated watch later list
from my phone just by being in the same network as each other. It
shouldn't necessitate the internet.

I should be able to store music from a variety of sources. I shouldn't
have to think about "if this is a URL, then I have to paste it
somewhere. If it's a file, then it's stuck on my computer."

The watch later stuff should be something like a queue. The list can
be arbitrarily large, but that doesn't mean that you wanna store all
of the list on every device. It would be better if it kept the 1st
100-200 things in the list. When you finish something, you have the
option of keeping it around because you like it, or it gets booted
from your device (and watch later list), thus making room for the next
item on the list to be loaded onto your device.

The most important feature is the ability to listen to music that may
not be locally on the device despite a spotty connection. It shouldn't
fail if a connection disappears. It should gracefully stop the current
track, shouldn't prevent you from playing the part that you've already
downloaded. If there's actual streaming of content going on, it should
prioritize parts that are near where you are now. I don't wanna
download the beginning and end of a song, leaving the middle empty.

It's definitely possible to allow other devices to get music in this
way. This would make it a music server, too. It could also allow for a
music server w/o connectiction to the internet. You could take a rasp
pi or phone with this software installed and allow guests to
listen/keep songs from the device just by being on the same network as
your device.

Music Sources that I'd like to be able to use:

- SoulSeek
- Torrent
- Streaming Sites (like Youtube, bandcamp)
- Local files
- Soundcloud
- Mixtape downloads (zip files)
- Spotify/Pandora, perhaps?

Use with more than one device simultaneously (eg listen to music on my
phone while adding new music to the collection on my computer).

I'd like to be able to modify metadata for the music without swapping
out a whole file.

Q: What happens if a site like youtube or soundcloud removes or
changes a song?
A: It might be best to specify a song with a URL, but then immediately
download the source and add it to the network.

Perhaps the best way to work with these disparate sources is to have
some kinda path that specifies how to get these files, immediately
download them, and then program everything else to work at the file
layer. The bulk of the program logic will just be filesyncing/sharing.

The real point of this application is *streaming*. Everything else is
about figuring out how to stream from different sources with a uniform
interface, and how to stream with spotty (but high-quality)
connections. This is specialized to being in the current MTA train
system, having LinkNYCFreeWifi stations around, going to work/college.
It's for when you know that you're going to be surrounded by
streaming-quality wifi, but *not at all times.*

However, the real point of what I want to do is to have one music
collection accessible from multiple locations, made up of disparate
sources of music, accessible remotely even from a connection which
will dip in and out, so long as the connection (when I have it) is
pretty fast (>=1MB/s), or from a constant but slow internet connection
(=~1Mb/s). One of the disparate sources must be files stored on my own
computers. Another one must be sources accessible from online, such as
streaming websites.

Next Steps
----------

- Look for existing solutions close to your whole project. Maybe
  there's a halfway-done abandoned project with similar aims.
    - Google Play Music
        - Summary: 
            - What it has: It allows me to listen to my own songs, and
              caches recently-listened-to music. Streams the files I
              want to share. 50K song limit. If I use this in a
              serious way, I'll hit this cap soon enough. I can also
              use this on a desktop with a GPM desktop player.
            - Lacking: The ability to search through disparate sources
              for songs, download them from those sources. It doesn't
              play music from my own local computers. It will store
              those files on Google's network. If their network goes
              down, if they stop the application's servers then my
              music stops. I can't add to the collection from my
              laptop (perhaps from my desktop).
        - <https://lifehacker.com/5824193/five-best-streaming-music-services>
        - Google Play Music might be close. Must look at this more. I
        don't want to pay, though, and I don't want ads.
        - <https://www.makeuseof.com/tag/google-play-music-features/>
        - For free, I can upload songs to their database and listen to
          it from anywhere. 50K song limit.
            - Format whitelist: MP3, AAC, WMA, FLAC, Ogg, and ALAC.
        - To stream songs from their collection, I have to pay (\$10
          month as of Fri Sep 21 16:14:37 EDT 2018).
        - I can't upload songs from disparate sources. I'd have to
          download them 1st.
        - I can download songs for offline play. I'm not sure I'll
          have the caching mechanism that I'd want, where I can go
          back to a song I just played a few minutes or hours ago.
        - <https://www.techrepublic.com/article/pro-tip-manage-google-play-music-for-easier-transition-between-online-and-offline-modes/>
          GPM does indeed cache recently-listened-to songs. I wonder
          how much space it will use?
    - <http://pansentient.com/2011/11/how-to-hear-your-local-music-anywhere-with-spotify-and-dropbox/>
        - Spotify might be capable of it, too, if combined with
          dropbox.  This news is 7 years old. TODO: Check if this
          still works.
- Look for "streaming filesystem". See if there's something that
  kinda/sorta acts like a filesystem where not everything is always
  stored on your computer. Like an fs cache where you can pull files
  from a bunch of places, but you don't have to have a huge drive to
  actually store it all. You can just pull files again when you need
  them.
    - <https://www.howtogeek.com/118075/how-to-stream-videos-and-music-over-the-network-using-vlc/>
- Closely related look up resources about creating example filesystems
    - <https://stackoverflow.com/questions/4714056/how-to-implement-a-very-simple-filesystem>
    - Above resource says to look at using
      [FUSE](https://github.com/libfuse/libfuse).
    - If I wanted to, I could do this with Racket:
      <https://docs.racket-lang.org/fuse/index.html>
    - Resources to consult about creating filesystems:
        - <http://web.mit.edu/6.033/1997/handouts/html/04sfs.html>
        - <http://www.cs.ucsb.edu/~chris/teaching/cs170/projects/proj5.html>

Existing Things that Can Satisfy Project Requirements
-----------------------------------------------------

- What would work/how:
- What would not work
- Other thoughts

What if I put all of my music onto one portable physical storage
device? Wouldn't this solve my problems?

- What would work/how:
    - Watch later: download everything. If you don't like it, delete
      it.
    - Syncing to diff devices: Plug in the storage device into the
      current device.
    - Operating with a spotty connection: Only for files already on
      the device.
- What would not work
    - Sharing: Just one device. If I try to have two, then I'll have
      to maintain both of them.
    - Working with non-local-file sources.
    - Use with more than one device at a time.
- Other thoughts
    - I could forget this at home, and that would suck.
    - It would be nice to listen on my phone and download on my laptop
      if I'm using both.
    - It would be nice to get some robustness for the collection. If I
      break the device, lose it, or forget it, I'm assed out of music.
      That would suck for a long trip with nothing to do. Which I
      might see more of soon if I wanna travel.
    - Though there could be devices that wouldn't work with portable
      storage, I'm not likely to use them.

What about SSHFS, or something like it?

- What would work/how:
    - Sharing: One computer would be a host computer (the SSH server)
      and other clients would connect to this host for the music
      files. This could also work over a local network.
    - Watch Later: Copy files from the SSHFS to the host device (thus
      requires device file access, which I don't think iOS has).
      Delete the stuff when you no longer want it.
    - Use with more than one device. You ought to be able to add files
      to the host computer's folder on one computer and stream from
      another device using SSHFS as well.
- What would not work
    - Working with multiple sources: This is sharing for local files.
    - Watch later list is not automatically queued for offline use,
      nor is it automatically cleaned out when you don't want
      something.
- Other thoughts
    - It might be worth looking into this if the project takes the
      form of a file system. The following are relevant questions that
      SSHFS could handle:
        - What happens when Multiple users reading the same file.
        - What happens when Multiple users writing the same file.
        - What happens when A user reading while another user writes.
        - What happens when Editing a text file on a computer (like,
          say, vim). Does it place a copy of the file on the local
          computer? How many of the files you want are actually on
          your computer at any given time?  Is it none, and a file is
          fetched via SSH and loaded into memory when you request it?
        - What happens when How would it handle spotty connection and
          roaming? One sshfs thing mentioned
          [mosh](http://mosh.mit.edu).
    - What happens if many people try to use the one SSH server? Maybe
      I get all my friends to pool our collection of music together.
      Then what happens if we're all try to copy the whole collection
      to a new computer, all simultaneously?

IPFS

- What would work/how:
    - Sharing: Make a folder available via IPFS.
    - Syncing on a local network: Part of IPFS.
- What would not work
- Other thoughts
    - Multiple Sources: might work, but not yet sure. If I use IPFS,
      then what I'm really sharing are URLs. If I make eveyrthing work
      via URLs, then I can work with any source that can work with
      URLs. Youtube, torrent files, other streaming sites. However,
      I'm not sure how I'd handle SoulSeek. Perhaps I could translate
      a URL into a particular file on a particular user's computer.

Other Candidates:
    - syncthing
        - This doesn't have an upper limit on files. So if I share the
          music, I'll share all of it.
        - This might be suited to streaming applications, because it
          uses something based on torrents to sync the files. There's
          torrent streaming, so perhaps the same can be done here.
    - rsync
    - maybe some form of p2p file system. Something like IPFS,
      actually.
        - This might be the most well-suited out of the candidates,
          but it might be tough to modify the files. Also, I don't
          know if I could do streaming immediately; I think I'd have
          to stream the file to a music player or something.
    - owncloud
        - This also shares all of the files, it seems. I'm not sure
          it's suited to streaming applications, either.
